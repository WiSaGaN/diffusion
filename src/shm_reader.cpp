// Created on October 26, 2013 by Lu, Wangshan.
#include <atomic>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <diffusion/factory.hpp>
namespace diffusion {
extern const Size kShmHeaderLength; // Search it in shm_reader.cpp
class ShmReader : public Reader {
public:
    ShmReader(std::string const & shm_name);
    virtual bool can_read();
    virtual ByteBuffer read();
    virtual void read(std::vector<char> &buffer);
private:
    boost::interprocess::shared_memory_object shm_object_;
    boost::interprocess::mapped_region shm_region_;
    // const since first asignment.
    char * shm_header_position_;
    char * shm_body_position_;
    Size shm_body_size_;
    // variable.
    std::uint32_t writer_shm_body_offset_;
    Offset reader_shm_body_offset_;
    ByteBuffer cyclic_read(Size size);
    void cyclic_read(std::vector<char> &buffer);
    void update_writer_shm_body_offset();
};
Reader * create_shared_memory_reader(std::string const & shm_name) {
    return new ShmReader(shm_name);
}
ShmReader::ShmReader(std::string const & shm_name)
    : shm_object_(boost::interprocess::open_only, shm_name.c_str(), boost::interprocess::read_only),
      shm_region_(shm_object_, boost::interprocess::read_only),
      reader_shm_body_offset_(0) {
    shm_header_position_ = reinterpret_cast<char *>(shm_region_.get_address());
    shm_body_position_ = shm_header_position_ + kShmHeaderLength;
    shm_body_size_ = shm_region_.get_size() - kShmHeaderLength;
    this->update_writer_shm_body_offset();
}
bool ShmReader::can_read() {
    if (reader_shm_body_offset_ != static_cast<Offset>(writer_shm_body_offset_)) {
        return true;
    } else {
        this->update_writer_shm_body_offset();
        if (reader_shm_body_offset_ != static_cast<Offset>(writer_shm_body_offset_)) {
            return true;
        } else {
            return false;
        }
    }
}
ByteBuffer ShmReader::read() {
    if (!this->can_read()) {
        throw ErrorNoData();
    }
    auto body_size_buffer = this->cyclic_read(sizeof(Size)); // avoid unaligned access.
    auto body_size = read_aligned_object<Size>(body_size_buffer.const_data());
    auto body = this->cyclic_read(body_size);
    return body;
}
void ShmReader::read(std::vector<char> &buffer) {
    if (!this->can_read()) {
        throw ErrorNoData();
    }
    buffer.resize(sizeof(Size));
    this->cyclic_read(buffer); // avoid unaligned access.
    auto body_size = read_aligned_object<Size>(buffer.data());
    buffer.resize(body_size);
    this->cyclic_read(buffer);
}
ByteBuffer ShmReader::cyclic_read(Size size) {
    ByteBuffer data(size);
    auto bytes_left = size;
    while (bytes_left > 0) {
        auto write_pointer = data.data() + size - bytes_left;
        auto space_left = shm_body_size_ - reader_shm_body_offset_;
        auto bytes_can_be_read_without_wrap = (bytes_left > space_left) ? space_left : bytes_left;
        std::memcpy(write_pointer, shm_body_position_ + reader_shm_body_offset_, bytes_can_be_read_without_wrap);
        reader_shm_body_offset_ += bytes_can_be_read_without_wrap;
        bytes_left -= bytes_can_be_read_without_wrap;
        if (reader_shm_body_offset_ == shm_body_size_) {
            reader_shm_body_offset_ = 0;
        }
    }
    return data;
}
void ShmReader::cyclic_read(std::vector<char> &buffer) {
    auto bytes_left = static_cast<Size>(buffer.size());
    while (bytes_left > 0) {
        auto write_pointer = buffer.data() + buffer.size() - bytes_left;
        auto space_left = shm_body_size_ - reader_shm_body_offset_;
        auto bytes_can_be_read_without_wrap = (bytes_left > space_left) ? space_left : bytes_left;
        std::memcpy(write_pointer, shm_body_position_ + reader_shm_body_offset_, bytes_can_be_read_without_wrap);
        reader_shm_body_offset_ += bytes_can_be_read_without_wrap;
        bytes_left -= bytes_can_be_read_without_wrap;
        if (reader_shm_body_offset_ == shm_body_size_) {
            reader_shm_body_offset_ = 0;
        }
    }
}
void ShmReader::update_writer_shm_body_offset() {
    auto writer_shm_body_offset_position = reinterpret_cast<std::atomic<std::uint32_t> *>(shm_header_position_);
    writer_shm_body_offset_ = std::atomic_load_explicit(writer_shm_body_offset_position, std::memory_order_acquire);
}
} // namespace diffusion


// Created on October 26, 2013 by Lu, Wangshan.
/// This is the implementation file of nostd::diffusion Shared Memory Writer.
/// For Writer and Reader maintainer:
/// Writer and Reader communicate with a shared memory file. File length is specified in writer's constructor.
/// Since the shared memory region contains a 8 byte header, the shared memory file length should not be less than 5.
/// The header contains a 4 byte signed integer in the begining indicates the writer's committed offset, starting immediately after header.
/// The reader can read the header as a 4 byte signed integer to know if there is available data.
/// For a RawData object, writer will write 4 bytes signed integer size of RawData's payload, followed by the payload of RawData.
/// The writer will wrap to the shared memroy region's payload's beginning when it encounters the end of the region.
/// That means if user specifies a length of 1000000 bytes in the writer's constructor, the payload region for writing data is 999996 bytes.
/// And if RawData contains a length of 28 bytes rawdata, it will take 4 + 28 = 32 bytes in the share memory payload region.
#include <atomic>
#include <memory>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <diffusion/factory.hpp>
namespace diffusion {
extern constexpr Size kShmHeaderLength = 8; // Also used by shm_reader.cpp
class ShmWriter : public Writer {
public:
    ShmWriter(std::string const &shm_name, Size shm_size);
    virtual ~ShmWriter();
    virtual void write(std::vector<char> const & data);
    virtual void write(char const *data, std::size_t size);
private:
    std::string shm_name_;
    boost::interprocess::shared_memory_object shm_object_;
    std::unique_ptr<boost::interprocess::mapped_region> shm_region_;
    // const since first asignment.
    char * shm_header_position_;
    char * shm_body_position_;
    Size shm_body_size_;
    // variable.
    std::uint32_t writer_shm_body_offset_;
    void cyclic_write(char const *data, std::size_t size);
    void commit();
};
Writer * create_shared_memory_writer(std::string const &shm_name, Size shm_size) {
    return new ShmWriter(shm_name, shm_size);
}
ShmWriter::ShmWriter(std::string const & shm_name, Size shm_size)
    : shm_name_(shm_name),
      shm_object_(boost::interprocess::create_only, shm_name.c_str(), boost::interprocess::read_write),
      writer_shm_body_offset_(0) {
    shm_object_.truncate(shm_size);
    shm_region_ = std::unique_ptr<boost::interprocess::mapped_region>(new boost::interprocess::mapped_region(shm_object_, boost::interprocess::read_write));
    shm_header_position_ = reinterpret_cast<char *>(shm_region_->get_address());
    shm_body_position_ = shm_header_position_ + kShmHeaderLength;
    shm_body_size_ = shm_region_->get_size() - kShmHeaderLength;
}
ShmWriter::~ShmWriter() {
    boost::interprocess::shared_memory_object::remove(shm_name_.c_str());
}
void ShmWriter::write(std::vector<char> const &data) {
    this->write(data.data(), data.size());
}
void ShmWriter::write(char const *data, std::size_t size) {
    auto size_prefix = static_cast<Size>(size);
    this->cyclic_write(reinterpret_cast<char const *>(&size_prefix), sizeof(size_prefix));
    this->cyclic_write(data, size);
    this->commit();
}
void ShmWriter::cyclic_write(char const *data, std::size_t size) {
    auto bytes_left = static_cast<Size>(size);
    while (bytes_left > 0) {
        auto start_position_unwritten_bytes = data + size - bytes_left;
        auto space_left = shm_body_size_ - static_cast<Size>(writer_shm_body_offset_);
        auto bytes_can_be_written_without_wrap = (bytes_left > space_left) ? space_left : bytes_left;
        std::memcpy(shm_body_position_ + static_cast<Offset>(writer_shm_body_offset_), start_position_unwritten_bytes, bytes_can_be_written_without_wrap);
        writer_shm_body_offset_ += bytes_can_be_written_without_wrap;
        bytes_left -= bytes_can_be_written_without_wrap;
        if (static_cast<Size>(writer_shm_body_offset_) == shm_body_size_) {
            writer_shm_body_offset_ = 0;
        }
    }
}
void ShmWriter::commit() {
    auto shm_offset_position = reinterpret_cast<std::atomic<std::uint32_t> *>(shm_header_position_);
    std::atomic_store_explicit(shm_offset_position, static_cast<std::uint32_t>(writer_shm_body_offset_), std::memory_order_release);
}
} // namespace diffusion


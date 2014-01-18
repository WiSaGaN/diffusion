// Created on November 20, 2013 by Lu, Wangshan.
#include <diffusion/factory.hpp>
#include <fstream>
#include <deque>
namespace diffusion {
extern const std::string kFileHeader;
class FileReader : public Reader {
public:
    FileReader(std::string const & file_name);
    virtual ~FileReader();
    virtual bool can_read();
    virtual ByteBuffer read();
private:
    std::ifstream file_;
    std::deque<ByteBuffer> data_queue_;
    bool is_file_valid();
};
FileReader::FileReader(std::string const & file_name)
    : file_(file_name) {
    // TODO: Add file checking.
    if (this->is_file_valid()) {
    } else {
    }
}
FileReader::~FileReader() {}
bool FileReader::can_read() {
    if (data_queue_.empty()) {
        auto file_position_before_reading = file_.tellg();
        ByteBuffer message_header(sizeof(Size));
        if (!file_.read(message_header.data(), message_header.size())) {
            // TODO: badbit throw read_fail.
            file_.seekg(file_position_before_reading);
            return false;
        } else {
            auto message_size = *reinterpret_cast<Size const *>(message_header.const_data());
            ByteBuffer message(message_size);
            if (!file_.read(message.data(), message.size())) {
                // TODO: badbit throw read_fail.
                file_.seekg(file_position_before_reading);
                return false;
            } else {
                data_queue_.push_back(message);
                return true;
            }
        }
    } else {
        return true;
    }
}
ByteBuffer FileReader::read() {
    if (this->can_read()) {
        ByteBuffer data = data_queue_.front();
        data_queue_.pop_front();
        return data;
    } else {
        throw ErrorNoData();
    }
}
bool FileReader::is_file_valid() {
    auto file_header_length = kFileHeader.length();
    std::string first_four_bytes(file_header_length, '0');
    if (!file_.read(&first_four_bytes[0], file_header_length)) {
        // TODO: badbit throw read_fail.
        return false;
    }
    if (first_four_bytes == kFileHeader) {
        return true;
    } else {
        return false;
    }
}
} // namespace diffusion

// Created on November 20, 2013 by Lu, Wangshan.
#include <diffusion/factory.hpp>
#include <fstream>
#include <deque>
#include <iostream> // Debug
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
Reader * create_file_reader(std::string const & file_name) {
    return new FileReader(file_name);
}
FileReader::FileReader(std::string const & file_name) {
    try {
        file_.open(file_name);
    } catch (std::exception const & e) {
        throw ErrorFileOpening();
    }
    if (!this->is_file_valid()) {
        throw ErrorDataCorruption();
    }
}
FileReader::~FileReader() {}
bool FileReader::can_read() {
    if (data_queue_.empty()) {
        auto file_position_before_reading = file_.tellg();
        ByteBuffer message_header(sizeof(Size));
        if (!file_.read(message_header.data(), message_header.size())) {
            if (file_.eof() && file_.gcount() == 0) {
                return false;
            } else {
                std::cout << "Aho, header error, now position " << file_.tellg() << ", just now " << file_position_before_reading << std::endl;
                throw ErrorDataCorruption();
            }
        } else {
            auto message_size = *reinterpret_cast<Size const *>(message_header.const_data());
            ByteBuffer message(message_size);
            if (!file_.read(message.data(), message.size())) {
                std::cout << "Aho, body error, now position " << file_.tellg() << ", just now " << file_position_before_reading << std::endl;
                throw ErrorDataCorruption();
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
    std::string actual_file_header(file_header_length, '0');
    if (!file_.read(&actual_file_header[0], file_header_length)) {
        return false;
    }
    if (actual_file_header == kFileHeader) {
        return true;
    } else {
        return false;
    }
}
} // namespace diffusion

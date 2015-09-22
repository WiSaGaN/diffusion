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
    virtual std::vector<char> read();
    virtual void read(std::vector<char> &buffer);
private:
    std::ifstream file_;
    std::deque<std::vector<char>> data_queue_;
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
        std::vector<char> message_header(sizeof(Size));
        if (!file_.read(message_header.data(), message_header.size())) {
            if (file_.eof() && file_.gcount() == 0) {
                return false;
            } else {
                throw ErrorDataCorruption();
            }
        } else {
            auto message_size = *reinterpret_cast<Size const *>(message_header.data());
            std::vector<char> message(message_size);
            if (!file_.read(message.data(), message.size())) {
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
std::vector<char> FileReader::read() {
    if (this->can_read()) {
        std::vector<char> data(data_queue_.front());
        data_queue_.pop_front();
        return data;
    } else {
        throw ErrorNoData();
    }
}
void FileReader::read(std::vector<char> &buffer) {
    if (this->can_read()) {
        buffer = data_queue_.front();
        data_queue_.pop_front();
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
    return actual_file_header == kFileHeader;
}
} // namespace diffusion

// Created on November 29, 2013 by Lu, Wangshan.
#include <fstream>
#include <diffusion/factory.hpp>
namespace diffusion {
extern const std::string kFileHeader = "DFSN";
class FileWriter : public Writer {
public:
    FileWriter(std::string const & file_name);
    virtual ~FileWriter();
    virtual void write(std::vector<char> const & data);
    virtual void write(char const *data, std::size_t size);
private:
    std::ofstream file_;
    void write_file_header();
};
Writer * create_file_writer(std::string const & file_name) {
    return new FileWriter(file_name);
}
FileWriter::FileWriter(std::string const & file_name) {
    try {
        file_.open(file_name);
    } catch (std::exception const & e) {
        throw ErrorFileOpening();
    }
    this->write_file_header();
}
FileWriter::~FileWriter() {
}

void FileWriter::write(const std::vector< char >& data) {
    this->write(data.data(), data.size());
}

void FileWriter::write(const char* data, std::size_t size) {
    auto size_prefix = static_cast<Size>(size);
    file_.write(reinterpret_cast<char const *>(&size_prefix), sizeof(size_prefix));
    file_.write(data, size);
}

void FileWriter::write_file_header() {
    file_.write(kFileHeader.data(), kFileHeader.length());
}
} // namespace diffusion

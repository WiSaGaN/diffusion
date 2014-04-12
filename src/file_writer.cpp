// Created on November 29, 2013 by Lu, Wangshan.
#include <fstream>
#include <diffusion/factory.hpp>
namespace diffusion {
extern const std::string kFileHeader = "DFSN";
class FileWriter : public Writer {
public:
    FileWriter(std::string const & file_name);
    virtual ~FileWriter();
    virtual void write(ByteBuffer const & data);
private:
    std::ofstream file_;
    void write_file_header();
};
FileWriter::FileWriter(std::string const & file_name)
    : file_(file_name) {
    // TODO: Add file checking.
    this->write_file_header();
}
FileWriter::~FileWriter() {
}
void FileWriter::write(ByteBuffer const & data) {
    ByteBuffer data_serialization = prefix(data, static_cast<Size>(data.size()));
    file_.write(data_serialization.const_data(), data_serialization.size());
}
void FileWriter::write_file_header() {
    file_.write(kFileHeader.data(), kFileHeader.length());
}
} // namespace diffusion

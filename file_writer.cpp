// Created on November 29, 2013 by Lu, Wangshan.
#include <diffusion/factory.hpp>
namespace diffusion {
class FileWriter : public Writer {
public:
    FileWriter(std::string const & file_name);
    virtual void add(ByteBuffer const & data);
private:
};
FileWriter::FileWriter(std::string const & file_name) {

}
void FileWriter::add(ByteBuffer const & data) {

}
} // namespace diffusion

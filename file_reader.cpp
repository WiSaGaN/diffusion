// Created on November 20, 2013 by Lu, Wangshan.
#include <diffusion/factory.hpp>
namespace diffusion {
class FileReader : public Reader {
public:
    FileReader(std::string const & file_name);
    virtual bool has_next();
    virtual ByteBuffer get_next();
private:
};
} // namespace diffusion

// Created on November 19, 2013 by Lu, Wangshan.
#include <diffusion/factory.hpp>
#include <boost/asio.hpp>
namespace diffusion {
class NetWriter : public Writer {
public:
    NetWriter(std::string const & listening_ip_address, std::uint16_t listening_port);
    virtual void write(ByteBuffer const & data);
private:
};
NetWriter::NetWriter(std::string const & listening_ip_address, std::uint16_t listening_port) {

}
void NetWriter::write(ByteBuffer const & data) {

}
} // namespace diffusion

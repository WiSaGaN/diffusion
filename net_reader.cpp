// Created on November 19, 2013 by Lu, Wangshan.
#include <boost/asio.hpp>
#include <diffusion/factory.hpp>
namespace diffusion {
class NetReader : public Reader {
public:
    NetReader(std::string const & listening_ip_address, std::uint16_t listening_port);
    virtual bool can_read();
    virtual ByteBuffer read();
private:
};
} // namespace diffusion

// Created on November 19, 2013 by Lu, Wangshan.
#include <diffusion/factory.hpp>
#include <thread>
#include <boost/asio.hpp>
namespace diffusion {
class Server {
public:
    Server(boost::asio::io_service & input_io_service)
      : io_service_(input_io_service) {}
    void shut_down() {
        die_ = true;
    }
private:
    bool die_;
    boost::asio::io_service & io_service_;
};
class NetWriter : public Writer {
public:
    NetWriter(std::uint16_t listening_port);
    virtual ~NetWriter();
    virtual void write(ByteBuffer const & data);
private:
    boost::asio::io_service io_service_;
    Server server_;
    std::thread server_thread_;
};
Writer * create_network_writer(std::uint16_t listening_port) {
    return new NetWriter(listening_port);
}
NetWriter::NetWriter(std::uint16_t listening_port)
  : server_(io_service_) {

}
NetWriter::~NetWriter() {
    server_.shut_down();
    server_thread_.join();
}
void NetWriter::write(ByteBuffer const & data) {
    auto serialization = prefix(data, data.size());
}
} // namespace diffusion

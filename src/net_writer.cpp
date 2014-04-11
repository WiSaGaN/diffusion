// Created on November 19, 2013 by Lu, Wangshan.
#include <diffusion/factory.hpp>
#include <thread>
#include <mutex>
#include <queue>
#include <boost/asio.hpp>
#include <boost/concept_check.hpp>
namespace diffusion {
class Server {
public:
    Server(boost::asio::io_service & input_io_service)
      : io_service_(input_io_service) {}
    void post(ByteBuffer && data) {
        if (!die_) {
            std::lock_guard<std::mutex> lock(mutex_data_queue_);
            data_queue_.push(std::move(data));
        }
    }
    void run() {
        while (!die_) {
            ByteBuffer data(0);
            {
                std::lock_guard<std::mutex> lock(mutex_data_queue_);
                if (!data_queue_.empty()) {
                    data = data_queue_.front();
                    data_queue_.pop();
                }
            }
            if (data.size() > 0) {
                std::cout.write(data.const_data(), data.size());
                std::cout << std::endl;
                // TODO: Send data to all connections.
            }
        }
    }
    void shut_down() {
        die_ = true;
    }
private:
    volatile bool die_;
    std::mutex mutex_data_queue_;
    std::queue<ByteBuffer> data_queue_;
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
  : server_(io_service_), server_thread_(&Server::run, &server_) {
}
NetWriter::~NetWriter() {
    server_.shut_down();
    server_thread_.join();
}
void NetWriter::write(ByteBuffer const & data) {
    auto serialization = prefix(data, data.size());
    server_.post(std::move(serialization));
}
} // namespace diffusion

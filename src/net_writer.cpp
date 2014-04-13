// Created on November 19, 2013 by Lu, Wangshan.
#include <diffusion/factory.hpp>
#include <thread>
#include <mutex>
#include <queue>
#include <set>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/concept_check.hpp>
namespace diffusion {
class Server;
class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::io_service & io_service, Server * server)
      : socket_(io_service), server_(server) {}
    boost::asio::ip::tcp::socket & socket() {
        return socket_;
    }
    void write(ByteBuffer const & buffer) {
        boost::asio::async_write(socket_,
            boost::asio::buffer(buffer.const_data(), buffer.size()),
            boost::bind(&Session::handle_write, this, boost::asio::placeholders::error));
    }
private:
    void handle_write(boost::system::error_code const & error);
    boost::asio::ip::tcp::socket socket_;
    char buffer_[1024];
    Server * server_;
};
class Server {
public:
    Server(boost::asio::io_service & input_io_service, std::uint16_t port)
      : io_service_(input_io_service),
        acceptor_(io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
            this->start_accept();
    }
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
                for (auto session : sessions_) {
                    session->write(data);
                }
            }
            io_service_.poll_one();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    void shut_down() {
        die_ = true;
    }
    void remove(std::shared_ptr<Session> session) {
        sessions_.erase(session);
    }
private:
    volatile bool die_;
    std::mutex mutex_data_queue_;
    std::queue<ByteBuffer> data_queue_;
    boost::asio::io_service & io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::set<std::shared_ptr<Session>> sessions_;
    void start_accept() {
        auto new_session = std::make_shared<Session>(io_service_, this);
        acceptor_.async_accept(new_session->socket(),
          boost::bind(&Server::handle_accept, this, new_session, boost::asio::placeholders::error()));
    }
    void handle_accept(std::shared_ptr<Session> new_session, boost::system::error_code const & error) {
        if (!error) {
            sessions_.insert(new_session);
        }
        this->start_accept();
    }
};
void Session::handle_write(boost::system::error_code const & error) {
    if (error) {
        server_->remove(shared_from_this());
        // Here this object has been destroyed, so nothing should be added here.
    }
}
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
  : server_(io_service_, listening_port), server_thread_(&Server::run, &server_) {
}
NetWriter::~NetWriter() {
    server_.shut_down();
    server_thread_.join();
}
void NetWriter::write(ByteBuffer const & data) {
    auto serialization = prefix(data, static_cast<Size>(data.size()));
    server_.post(std::move(serialization));
}
} // namespace diffusion

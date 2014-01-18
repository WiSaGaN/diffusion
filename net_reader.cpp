// Created on November 19, 2013 by Lu, Wangshan.
#include <diffusion/factory.hpp>
#include <deque>
#include <atomic>
#include <mutex>
#include <memory>
#include <thread>
#include <boost/asio.hpp>
namespace diffusion {
template<typename Data>
class SyncQueue {
public:
    void push(Data const & data) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        queue_.push_back(data);
    }
    bool pop(Data & data) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (queue_.empty()) {
            return false;
        } else {
            data = queue_.front();
            queue_.pop_front();
            return true;
        }
    }
    bool is_empty() const {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        return queue_.empty();
    }
private:
    mutable std::mutex queue_mutex_;
    std::deque<Data> queue_;
};
class Receiver {
public:
    Receiver(boost::asio::ip::tcp::socket && input_socket, SyncQueue<ByteBuffer> & message_queue)
        : die_(false),
          socket_(std::move(input_socket)),
          message_queue_(message_queue) {
    }
    ~Receiver() {
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
        socket_.close();
    }
    void operator()() {
        while (!die_) {
            // read, or wait for 500ms.
        }
    }
    void shut_down() const {
        die_ = true;
    }
private:
    mutable std::atomic<bool> die_;
    boost::asio::ip::tcp::socket socket_;
    SyncQueue<ByteBuffer> & message_queue_;
};
class NetReader : public Reader {
public:
    NetReader(std::string const & listening_ip_address, std::uint16_t listening_port);
    virtual ~NetReader();
    virtual bool can_read();
    virtual ByteBuffer read();
private:
    std::unique_ptr<Receiver> receiver_;
    std::thread receiver_thread_;
    SyncQueue<ByteBuffer> message_queue_;
};
NetReader::NetReader(std::string const & listening_ip_address, std::uint16_t listening_port) {
    // Connect.
    // Create socket.
    // Create Receiver.
    // Create thread.
}
NetReader::~NetReader() {
    receiver_->shut_down();
    receiver_thread_.join();
}
bool NetReader::can_read() {
    return !message_queue_.is_empty();
}
ByteBuffer NetReader::read() {
    if (this->can_read()) {
        // Assuming single thread.
        ByteBuffer data(0);
        message_queue_.pop(data);
        return data;
    } else {
        throw ErrorNoData();
    }
}
} // namespace diffusion

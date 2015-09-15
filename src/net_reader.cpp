// Created on November 19, 2013 by Lu, Wangshan.
#include <diffusion/factory.hpp>
#include <deque>
#include <atomic>
#include <mutex>
#include <memory>
#include <thread>
#include <boost/asio.hpp>
// TODO: Move exception from worker thread to thread calling can_read.
namespace diffusion {
class Tokenizer {
public:
    void push(std::string const & raw_data) {
        unprocessed_data_ += raw_data;
        auto message_length = *reinterpret_cast<Size *>(&unprocessed_data_[0]);
        unprocessed_data_.erase(0, sizeof(Size));
        if (message_length < 0)
            throw ErrorDataCorruption();
        if (static_cast<std::size_t>(message_length) > unprocessed_data_.size()) {
        } else {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            processed_data_queue_.push_back(to_vector_char(&unprocessed_data_[0], message_length));
            unprocessed_data_.erase(0, message_length);
        }
    }
    bool pop(ByteBuffer & data) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (processed_data_queue_.empty()) {
            return false;
        } else {
            auto vector_data = processed_data_queue_.front();
            data = ByteBuffer(vector_data.data(), vector_data.size());
            processed_data_queue_.pop_front();
            return true;
        }
    }
    bool pop(std::vector<char> &data) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (processed_data_queue_.empty()) {
            return false;
        } else {
            data = processed_data_queue_.front();
            processed_data_queue_.pop_front();
            return true;
        }
    }
    bool is_empty() const {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        return processed_data_queue_.empty();
    }
private:
    std::string unprocessed_data_;
    mutable std::mutex queue_mutex_;
    std::deque<std::vector<char>> processed_data_queue_;
};
class Receiver {
public:
    Receiver(boost::asio::ip::tcp::socket && input_socket, Tokenizer & data_queue)
        : die_(false),
          socket_(std::move(input_socket)),
          data_queue_(data_queue) {
        socket_.shutdown(boost::asio::socket_base::shutdown_send);
    }
    ~Receiver() {
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
        socket_.close();
    }
    void run() {
        char buffer[65536];
        while (!die_) {
            std::size_t length = 0;
            try {
                length = socket_.receive(boost::asio::buffer(buffer));
            } catch (std::exception const & e) {
                throw ErrorDisconnection();
            }
            if (length > 0) {
                data_queue_.push(std::string(buffer, length));
            }
        }
    }
    void shut_down() const {
        die_ = true;
    }
private:
    mutable std::atomic<bool> die_;
    boost::asio::ip::tcp::socket socket_;
    Tokenizer & data_queue_;
};
class NetReader : public Reader {
public:
    NetReader(std::string const & destination_ip_address, std::string destination_port);
    virtual ~NetReader();
    virtual bool can_read();
    virtual ByteBuffer read();
    virtual void read(std::vector<char> &buffer);
private:
    std::shared_ptr<Receiver> receiver_;
    std::thread receiver_thread_;
    Tokenizer data_queue_;
};
Reader * create_network_reader(std::string const & writer_host, std::string const & writer_port) {
    return new NetReader(writer_host, writer_port);
}
NetReader::NetReader(std::string const & destination_ip_address, std::string destination_port) {
    boost::asio::io_service io_service;;
    boost::asio::ip::tcp::socket socket(io_service);
    boost::asio::ip::tcp::resolver resolver(io_service);
    try {
        boost::asio::connect(socket, resolver.resolve({destination_ip_address, destination_port}));
    } catch (std::exception const & e) {
        throw ErrorNoWriter();
    }
    receiver_ = std::shared_ptr<Receiver>(new Receiver(std::move(socket), data_queue_));
    receiver_thread_ = std::thread([=](){receiver_->run();});
}
NetReader::~NetReader() {
    receiver_->shut_down();
    receiver_thread_.join();
}
bool NetReader::can_read() {
    return !data_queue_.is_empty();
}
ByteBuffer NetReader::read() {
    if (this->can_read()) {
        // Assuming single thread.
        ByteBuffer data(0);
        data_queue_.pop(data);
        return data;
    } else {
        throw ErrorNoData();
    }
}
void NetReader::read(std::vector<char> &buffer) {
    if (this->can_read()) {
        // Assuming single thread.
        std::vector<char> data(0);
        data_queue_.pop(data);
        buffer = data;
    } else {
        throw ErrorNoData();
    }
}

} // namespace diffusion

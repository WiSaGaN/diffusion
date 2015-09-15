// Created on August 27, 2013 by Lu, Wangshan.
// This file contains bytes manipulation functions and classes. They are unsafe by nature. So use it with cautious, and put them in a safe wrapper early.
#ifndef DIFFUSION_BYTE_BUFFER_HPP_ // Macro used in byte_printer.hpp.
#define DIFFUSION_BYTE_BUFFER_HPP_
#include <cstring>
#include <string>
#include <vector>
namespace diffusion {
class ByteBuffer {
public:
    explicit ByteBuffer(std::size_t size) : data_(new char[size]), size_(size) {}
    ByteBuffer(char const * data, std::size_t size) : data_(new char[size]), size_(size) { std::memcpy(data_, data, size); }
    explicit ByteBuffer(std::string const & data) : data_(new char[data.size()]), size_(data.size()) { std::memcpy(data_, data.data(), data.size()); }
    explicit ByteBuffer(std::vector<char> const &data) : ByteBuffer(data.data(), data.size()) {}
    ByteBuffer(ByteBuffer const & lvalue_buffer) : data_(new char[lvalue_buffer.size()]), size_(lvalue_buffer.size()) {
        std::memcpy(data_, lvalue_buffer.const_data(), lvalue_buffer.size());
    }
    ByteBuffer(ByteBuffer && rvalue_buffer) {
        data_ = rvalue_buffer.data_;
        rvalue_buffer.data_ = nullptr;
        size_ = rvalue_buffer.size_;
        rvalue_buffer.size_ = 0;
    }
    ByteBuffer & operator=(ByteBuffer const & lvalue_buffer) {
        auto temp = new char[lvalue_buffer.size()]; // If this throws, the object remains intact. (Strong exception safety)
        delete[] data_;
        data_ = temp;
        size_ = lvalue_buffer.size();
        std::memcpy(data_, lvalue_buffer.const_data(), lvalue_buffer.size());
        return *this;
    }
    ByteBuffer & operator=(ByteBuffer && rvalue_buffer) {
        delete[] data_;
        data_ = rvalue_buffer.data_;
        rvalue_buffer.data_ = nullptr;
        size_ = rvalue_buffer.size_;
        rvalue_buffer.size_ = 0;
        return *this;
    }
    ~ByteBuffer() {
        delete[] data_;
        data_ = nullptr;
        size_ = 0;
    }
    void reallocate(std::size_t size) {
        auto temp = new char[size]; // If this throws, the object remains intact. (Strong exception safety)
        delete[] data_;
        data_ = temp;
        size_ = size;
    }
    void clear() { std::memset(data_, 0, size_); }
    char * data() {
        return data_;
    }
    char const * const_data() const { return data_; }
    std::size_t size() const { return size_; }
private:
    char * data_;
    std::size_t size_;
};
inline ByteBuffer to_byte_buffer(std::string const & data) {
    return ByteBuffer(data.data(), data.size());
}
} // namespace diffusion
#endif // DIFFUSION_BYTE_BUFFER_HPP_

// Created on October 26, 2013 by Lu, Wangshan.
#ifndef DIFFUSION_TYPES_HPP_
#define DIFFUSION_TYPES_HPP_
#include <cstdint>
#include <stdexcept>
#include <diffusion/byte_buffer.hpp>
#include <diffusion/byte_writer_reader.hpp>
namespace diffusion {
typedef std::int32_t Size;
typedef std::int32_t Offset;
template<typename POD>
ByteBuffer prefix(ByteBuffer const & data, POD const & object) {
    ByteBuffer memory_object(sizeof(object) + data.size());
    auto write_pointer = memory_object.data();
    write_pointer += write_aligned_object(write_pointer, object);
    std::memcpy(write_pointer, data.const_data(), data.size());
    return memory_object;
}
class ErrorNoWriter : public std::runtime_error {
public:
    ErrorNoWriter() : std::runtime_error("Diffusion: There is no writer.") {
    }
class ErrorNoData : public std::runtime_error {
public:
    ErrorNoData() : std::runtime_error("Diffusion: There is no data to get.") {
    }
};
class ErrorDataCorruption : public std::runtime_error {
public:
    ErrorDataCorruption() : std::runtime_error("Diffusion: Data is corrupted.") {
    }
};
class ErrorDisconnection : public std::runtime_error {
public:
    ErrorDisconnection() : std::runtime_error("Diffusion: Disconnected.") {
    }
};
} // namespace diffusion

#endif // DIFFUSION_TYPES_HPP_

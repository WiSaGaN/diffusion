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
inline ByteBuffer serialize(ByteBuffer const & data) {
    ByteBuffer memory_object(sizeof(Size) + data.size());
    auto write_pointer = memory_object.data();
    write_pointer += write_aligned_object(write_pointer, data.size());
    std::memcpy(write_pointer, data.const_data(), data.size());
    return memory_object;
}
class ErrorNoData : public std::runtime_error {
public:
    ErrorNoData() : std::runtime_error("diffusion: There is no data to get.") {
    }
};
class ErrorDataCorruption : public std::runtime_error {
public:
    ErrorDataCorruption() : std::runtime_error("diffusion: Data is corrupted.") {
    }
};
} // namespace diffusion

#endif // DIFFUSION_TYPES_HPP_

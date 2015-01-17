// Created on November 26, 2013 by Lu, Wangshan.
#ifndef DIFFUSION_BYTE_PRINTER_HPP_
#define DIFFUSION_BYTE_PRINTER_HPP_
#include <iostream>
#include <iomanip>
#include <string>
namespace diffusion {
inline void print_bytes(char const * buffer, std::size_t count, std::size_t bytes_per_line, std::ostream & out) {
    std::ios::fmtflags flags(out.flags()); // Save flags before manipulation.
    out << std::hex << std::setfill('0');
    out.setf(std::ios::uppercase);
    for (std::size_t i = 0; i != count; ++i) {
        auto current_byte_number = static_cast<unsigned int>(static_cast<unsigned char>(buffer[i]));
        out << std::setw(2) << current_byte_number;
        bool is_end_of_line = (bytes_per_line != 0) && ((i + 1 == count) || ((i + 1) % bytes_per_line == 0));
        out << (is_end_of_line ? '\n' : ' ');
    }
    out.flush();
    out.flags(flags); // Restore original flags.
}
inline void print_bytes(char const * buffer, std::size_t count) {
    const std::size_t kBytesPerLine = 16;
    print_bytes(buffer, count, kBytesPerLine, std::cout);
}
inline void print_bytes(std::string const & buffer) { print_bytes(buffer.data(), buffer.size()); }

#ifdef DIFFUSION_BYTE_BUFFER_HPP_
inline void print_bytes(ByteBuffer const & buffer) { print_bytes(buffer.const_data(), buffer.size()); }
#endif // DIFFUSION_BYTE_BUFFER_HPP_

} // namespace diffusion
#endif // DIFFUSION_BYTE_PRINTER_HPP_

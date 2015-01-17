// Created on November 26, 2013 by Lu, Wangshan
#ifndef DIFFUSION_BYTE_WRITER_READER_HPP_
#define DIFFUSION_BYTE_WRITER_READER_HPP_
#include <cstring>
#include <string>
#include <type_traits>
namespace diffusion {
template<typename POD>
std::size_t write_aligned_object(char * byte_pointer, POD const & object) {
    static_assert(std::is_pod<POD>::value, "typename POD must be a POD type");
    *reinterpret_cast<POD *>(byte_pointer) = object;
    return sizeof(object);
}
template<typename POD>
POD read_aligned_object(char const * byte_pointer) {
    static_assert(std::is_pod<POD>::value, "typename POD must be a POD type");
    return *reinterpret_cast<POD const *>(byte_pointer);
}
class ByteWriter {
public:
    explicit ByteWriter(char * byte_pointer) : byte_pointer_(byte_pointer) {}
    void reset(char * byte_pointer) { byte_pointer_ = byte_pointer; }
    char * get_pointer() const { return byte_pointer_; }
    /// \brief This template function writes a POD datetype to the memory.
    /// \detail This function assumes the write can be unaligned, so it uses memcpy to avoid the alignment issue.
    template<typename POD>
    void write_object(POD const & object) {
        static_assert(std::is_pod<POD>::value, "typename POD must be a POD type");
        std::memcpy(byte_pointer_, &object, sizeof(object)); // Avoid alignment and aliasing issue.
        byte_pointer_ += sizeof(object);
    }
    void write_string(std::string const & string) {
        std::memcpy(byte_pointer_, string.data(), string.size());
        byte_pointer_ += string.size();
    }
private:
    char * byte_pointer_;
};
class ByteReader {
public:
    explicit ByteReader(char const * byte_pointer) : byte_pointer_(byte_pointer) {}
    void reset(char const * byte_pointer) { byte_pointer_ = byte_pointer; }
    char const * get_pointer() const { return byte_pointer_; }
    /// \brief This template function reads a POD datetype from the memory.
    /// \detail This function assumes the read can be unaligned, so it uses memcpy to avoid the alignment issue.
    template<typename POD>
    POD read_object() {
        static_assert(std::is_pod<POD>::value, "typename POD must be a POD type");
        POD object;
        std::memcpy(&object, byte_pointer_, sizeof(object)); // Avoid alignment and aliasing issue.
        byte_pointer_ += sizeof(object);
        return object;
    }
    std::string read_string(std::size_t size) {
        std::string result = std::string(byte_pointer_, size);
        byte_pointer_ += size;
        return result;
    }
private:
    char const * byte_pointer_;
};
} // namespace diffusion
#endif // DIFFUSION_BYTE_WRITER_READER_HPP_

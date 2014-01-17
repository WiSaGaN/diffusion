// Created on October 30, 2013 by Lu, Wangshan.
#ifndef DIFFUSION_FACTORY_HPP_
#define DIFFUSION_FACTORY_HPP_
#include <diffusion/types.hpp>
namespace diffusion {
/// Abstract base class that parents shm_writer, net_writer, and file_writer.
class Writer {
public:
    /// \brief Write data to medium. Medium can be shared memory, TCP/IP socket, or diffusion file.
    virtual void write(ByteBuffer const & data) = 0;
    /// \brief Virtual destructor for run-time polymorphism.
    virtual ~Writer() {}
};
/// Abstract base class that parents shm_reader, net_reader, and file_reader.
class Reader {
public:
    /// \brief Check if there is available data.
    /// \return true if it has, false if it is empty.
    virtual bool has_next() = 0;
    /// \brief Get next RawData if it is available.
    /// \details If there is available data, return available data as RawData.
    ///          If there is no available data, throw ErrorNoData exception.
    ///          To avoid throwing exception, one should always call has_next() before get_next().
    ///          Will throw ErrorDataCorruption if data is corrupted.
    /// \return the available RawData in the medium.
    virtual ByteBuffer read_next() = 0;
    /// \brief Virtual destructor for run-time polymorphism.
    virtual ~Reader() {}
};
class Factory {
public:
    /// \brief Create a new shared memory writer.
    /// \details Create a pointer to a newed shared memory writer. 
    ///          User should delete it after it is done.
    ///          Or use smart pointers to take care of it.
    /// \param shm_name specifies the name of the shared memory object, the reader will need the name to read the data.
    ///                 The object will be removed if it exists already. It will also be removed when writer is destroyed.
    ///                 Since the persistence of shared memory is kernel wise, and it will not be destoryed when there is still process accessing it.
    ///                 So when before restart the writer, one should stop all readers and writers. Otherwise it will not be able to create the same name object again.
    /// \param shm_size specifies the size of the shared memory object. 
    ///                 It should be larger than 64K bytes and needs to be less than 4 GiB, and subjects to memory limitation of the machine.
    ///                 It should also have a proper alignment of the machine, most of time it should be a multiple of 8.
    /// \return returns a pointer to a newed writer object.
    static Writer * create_shared_memory_writer(std::string const & shm_name, Size shm_size);
    /// \brief Create a new shared memory reader.
    /// \details Create a pointer to a newed shared memory writer.
    ///          User should delete it after it is done.
    ///          Or use smart pointers to take care of it.
    /// \param shm_name specifies the name of the shared memory object to read. The object must be already created by writer when this function is called.
    /// \return returns a pointer to a newed reader object.
    static Reader * create_shared_memory_reader(std::string const & shm_name);
    /// \brief Create a new network writer.
    /// \details Create a pointer to a newed network writer.
    ///          User should delete it after it is done.
    ///          Or use smart pointers to take care of it.
    static Writer * create_network_writer(std::string const & listening_ip, std::uint16_t listening_port);
    /// \brief Create a new network reader.
    /// \details Create a pointer to a newed network writer.
    ///          User should delete it after it is done.
    ///          Or use smart pointers to take care of it.
    /// \param writer_host is corresponding writer's internet host address.
    /// \param writer_port is corresponding writer's listening port.
    static Reader * create_network_reader(std::string const & writer_host, std::uint16_t writer_port);
};
} // namespace diffusion
#endif // DIFFUSION_FACTORY_HPP_

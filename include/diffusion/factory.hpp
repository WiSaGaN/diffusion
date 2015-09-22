// Created on October 30, 2013 by Lu, Wangshan.
#ifndef DIFFUSION_FACTORY_HPP_
#define DIFFUSION_FACTORY_HPP_
#include <diffusion/types.hpp>
namespace diffusion {
/// Abstract base class that parents shm_writer, net_writer, and file_writer.
class Writer {
public:
    /// \brief Write data to medium. Medium can be shared memory, TCP/IP socket, or diffusion file.
    virtual void write(std::vector<char> const &data) = 0;
    virtual void write(char const *data, std::size_t size) = 0;
    /// \brief Virtual destructor for run-time polymorphism.
    virtual ~Writer() {}
};
/// Abstract base class that parents shm_reader, net_reader, and file_reader.
class Reader {
public:
    /// \brief Check if there is available data.
    /// \details Will throw exception when exceptional situation happens,
    ///          such as network disconnection, data corruption. 
    /// \return true if it has, false if it is empty.
    virtual bool can_read() = 0;
    /// \brief Get next message if it is available.
    /// \details If there is available data, return available data as message.
    ///          Never throw when there is available data.
    ///          If there is no available data, throw ErrorNoData exception.
    ///          To avoid throwing exception, one should always call can_read() before read().
    /// \return the available message in the medium.
    virtual std::vector<char> read() = 0;
    virtual void read(std::vector<char> &buffer) = 0;
    /// \brief Virtual destructor for run-time polymorphism.
    virtual ~Reader() {}
};
/// \brief Create a new shared memory writer.
/// \details Create a pointer to a newed shared memory writer.
///          User should delete it after it is done.
///          Or use smart pointers to take care of it.
///          Will throw exception if shared memory can not be created.
/// \param shm_name specifies the name of the shared memory object, the reader will need the name to read the data.
///                 The object will be removed if it exists already. It will also be removed when writer is destroyed.
///                 Since the persistence of shared memory is kernel wise, and it will not be destoryed when there is still process accessing it.
///                 So when before restart the writer, one should stop all readers and writers. Otherwise it will not be able to create the same name object again.
/// \param shm_size specifies the size of the shared memory object.
///                 It should be larger than 64K bytes and needs to be less than 4 GiB, and subjects to memory limitation of the machine.
///                 It should also have a proper alignment of the machine, most of time it should be a multiple of 8.
/// \return returns a pointer to a newed writer object.
Writer * create_shared_memory_writer(std::string const & shm_name, Size shm_size);
/// \brief Create a new shared memory reader.
/// \details Create a pointer to a newed shared memory writer.
///          User should delete it after it is done.
///          Or use smart pointers to take care of it.
/// \param shm_name specifies the name of the shared memory object to read. The object must be already created by writer when this function is called.
/// \return returns a pointer to a newed reader object.
Reader * create_shared_memory_reader(std::string const & shm_name);
/// \brief Create a new network writer.
/// \details Create a pointer to a newed network writer.
///          User should delete it after it is done.
///          Or use smart pointers to take care of it.
//           Will throw exception when listening port cannot be opened.
Writer * create_network_writer(std::uint16_t listening_port);
/// \brief Create a new network reader.
/// \details Create a pointer to a newed network writer.
///          User should delete it after it is done.
///          Or use smart pointers to take care of it.
///          Will throw exception if connection can not be established.
/// \param writer_host is corresponding writer's internet host address.
/// \param writer_port is corresponding writer's listening port.
Reader * create_network_reader(std::string const & writer_host, std::string const & writer_port);
/// \brief Create a new file writer.
/// \details Create a pointer to a newed file writer.
///          User should delete it after it is done.
///          Or use smart pointers to take care of it.
///          Will throw exception if file can not be opened for writing.
/// \param file_name is the diffusion file that is about to write.
Writer * create_file_writer(std::string const & file_name);
/// \brief Create a new file reader.
/// \details Create a pointer to a newed file writer.
///          User should delete it after it is done.
///          Or use smart pointers to take care of it.
///          Will throw exception if file can not be opened, or file is not valid DFSN file.
/// \param file_name is the diffusion file that is about to read.
Reader * create_file_reader(std::string const & file_name);
} // namespace diffusion
#endif // DIFFUSION_FACTORY_HPP_

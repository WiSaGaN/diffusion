Diffusion
=========
**Diffusion** is a `C++` library for efficient message-based data distribution from one writer process to multiple reader processes. Currently Diffusion has three forms: to/from file, through shared memory, through network. Design of diffusion focuses on simplicity and efficiency.


### File
File form Diffusion writes to and read from a file format called diffusion file. It can be used to store message-based data, and later load them while preserving the message boundary.
#### Diffusion file
Diffusion file starts with a four-byte header of magic string `DFSN`, followed by a sequence of *messages*. Each *message* has a four-byte header of little-endian signed integer representing the payload size, and a payload. This implementation detail only matters when user wish to use diffusion file directly. 

### Shared Memory
Shared memory form Diffusion writer can write to a shared memory area, and let multiple shared memory readers read it concurrently without additional synchronization. It has very low latency, and is suitable for efficient data distribution between processes.

### Network
Network form Diffusion writer is a server waiting for network form Diffusion reader to connect. It has higher latency then shared memory, but can be used across network.

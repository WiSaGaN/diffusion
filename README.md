[![Build Status](https://travis-ci.org/WiSaGaN/diffusion.svg?branch=master)](https://travis-ci.org/WiSaGaN/diffusion)
[![Circle CI](https://circleci.com/gh/WiSaGaN/diffusion.svg?style=svg)](https://circleci.com/gh/WiSaGaN/diffusion)
Diffusion
=========
**Diffusion** is a `C++` library for efficient message-based data distribution from one writer process to multiple reader processes. Currently Diffusion has three forms: to/from file, through shared memory, through network. Design of diffusion focuses on simplicity and efficiency.


Getting Started
---------------

`Diffusion` uses `CMake` to generate building and installation scripts. Default installation is static; only headers and static debug and release libraries will be installed to the system library. So the resulting object files will not be dependent to `Diffusion` on target machine. User can specify to use dynamic libraries instead.

#### How to install on GNU/Linux?
After `Diffusion` is copied, and present working directory is set to directory `diffusion`. We set up a temporary directory for out-of-source build.

    mkdir build
    cd build

If we want to install in the default `/usr/local` directory, we can simply use

    cmake ..

We can also specify the directory where headers and library files will be installed

    cmake .. -DCMAKE_INSTALL_PREFIX=../some_directory_you_like

Then `Makefile` is generated, we can use

    make

to build the library.
Or to build and install the library to previously specified location.

    sudo make install

if we wish to install into the default `/usr/local` directory or other system directory. 

#### How to generate documentation?
`Diffusion` uses `Doxygen` to generate documentation for the library. Under `leanfix` directory, use

    doxygen

to generate documentation into `doc/` subdirectory. `HTML` version of the documentation can be then accessed by loading `doc/html/index.html` into the browser.


Reference
---------

### File
File form Diffusion writes to and read from a file format called diffusion file. It can be used to store message-based data, and later load them while preserving the message boundary.
#### Diffusion file
Diffusion file starts with a four-byte header of magic string `DFSN`, followed by a sequence of *messages*. Each *message* has a four-byte header of little-endian signed integer representing the payload size, and a payload. This implementation detail only matters when user wish to use diffusion file directly. 

### Shared Memory
Shared memory form Diffusion writer can write to a shared memory area, and let multiple shared memory readers read it concurrently without additional synchronization. It has very low latency, and is suitable for efficient data distribution between processes.

### Network
Network form Diffusion writer is a server waiting for network form Diffusion reader to connect. It has higher latency then shared memory, but can be used across network.

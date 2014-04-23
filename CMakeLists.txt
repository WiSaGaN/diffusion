cmake_minimum_required(VERSION 2.6)
project(diffusion)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
if(UNIX)
	if(CMAKE_COMPILER_IS_GNUCC)
		add_definitions(-Wall -std=c++0x)
	endif(CMAKE_COMPILER_IS_GNUCC)
endif(UNIX)
find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_library(diffusion src/shm_writer.cpp src/shm_reader.cpp src/file_writer.cpp src/file_reader.cpp src/net_writer.cpp src/net_reader.cpp)

add_subdirectory(test)

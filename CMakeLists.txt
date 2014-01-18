cmake_minimum_required(VERSION 2.6)
project(diffusion)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
if(UNIX)
	if(CMAKE_COMPILER_IS_GNUCC)
		add_definitions(-Wall -std=c++0x)
	endif(CMAKE_COMPILER_IS_GNUCC)
endif(UNIX)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_library(libdiffusion shm_writer.cpp shm_reader.cpp file_writer.cpp file_reader.cpp)

	

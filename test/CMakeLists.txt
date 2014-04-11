if (GTEST_FOUND)
	include_directories(${GTEST_INCLUDE_DIRS})
endif (GTEST_FOUND)

add_executable(t_net_writer t_net_writer.cpp)
target_link_libraries(t_net_writer diffusion ${Boost_LIBRARIES} pthread)

add_executable(t_net_reader t_net_reader.cpp)
target_link_libraries(t_net_reader diffusion ${Boost_LIBRARIES} pthread)

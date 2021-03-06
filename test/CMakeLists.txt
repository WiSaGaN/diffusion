find_package(GTest)
if (GTEST_FOUND)
	include_directories(${GTEST_INCLUDE_DIRS})
	add_executable(ut_file ut_file.cpp)
	target_link_libraries(ut_file diffusion ${Boost_LIBRARIES} ${GTEST_LIBRARIES} pthread)
    message("--> Unit tests added.")
else (GTEST_FOUND)
    message("--> Unit tests skipped.")
endif (GTEST_FOUND)

add_executable(t_shm_writer t_shm_writer.cpp)
target_link_libraries(t_shm_writer diffusion ${Boost_LIBRARIES} pthread rt)

add_executable(t_shm_reader t_shm_reader.cpp)
target_link_libraries(t_shm_reader diffusion ${Boost_LIBRARIES} pthread rt)

add_executable(t_net_writer t_net_writer.cpp)
target_link_libraries(t_net_writer diffusion ${Boost_LIBRARIES} pthread)

add_executable(t_net_reader t_net_reader.cpp)
target_link_libraries(t_net_reader diffusion ${Boost_LIBRARIES} pthread)

add_executable(t_file_writer t_file_writer.cpp)
target_link_libraries(t_file_writer diffusion ${Boost_LIBRARIES} pthread)

add_executable(t_file_reader t_file_reader.cpp)
target_link_libraries(t_file_reader diffusion ${Boost_LIBRARIES} pthread)

add_executable(pt_shm_throughput pt_shm_throughput.cpp)
target_link_libraries(pt_shm_throughput diffusion ${Boost_LIBRARIES} pthread rt)

add_executable(pt_shm_latency pt_shm_latency.cpp)
target_link_libraries(pt_shm_latency diffusion ${Boost_LIBRARIES} pthread rt)

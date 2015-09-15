// Created on April 16, 2014 by Lu, Wangshan.
#include <iostream>
#include <memory>
#include <thread>
#include <diffusion/factory.hpp>
int main(int argc, char * argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " shm_name" << std::endl;
        return 0;
    }
    auto shm_reader = std::shared_ptr<diffusion::Reader>(diffusion::create_shared_memory_reader(argv[1]));
    while (true) {
        if (shm_reader->can_read()) {
            auto line = shm_reader->read();
            std::cout.write(line.data(), line.size());
            std::cout << std::endl;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    return 0;
}

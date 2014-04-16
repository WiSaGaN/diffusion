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
    auto net_reader = std::shared_ptr<diffusion::Reader>(diffusion::create_shared_memory_reader(argv[1]));
    while (true) {
        if (net_reader->can_read()) {
            auto line = net_reader->read();
            std::cout.write(line.const_data(), line.size());
            std::cout << std::endl;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    return 0;
}

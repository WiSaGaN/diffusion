// Created on April 16, 2014 by Lu, Wangshan.
#include <iostream>
#include <memory>
#include <thread>
#include <diffusion/factory.hpp>
int main(int argc, char * argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " diffusion_file" << std::endl;
        return 0;
    }
    auto file_reader = std::shared_ptr<diffusion::Reader>(diffusion::create_file_reader(argv[1]));
    while (file_reader->can_read()) {
        auto line = file_reader->read();
        std::cout.write(line.const_data(), line.size());
        std::cout << std::endl;
    }
    return 0;
}

// Created on April 61, 2014 by Lu, Wangshan.
#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include <diffusion/factory.hpp>
int main(int argc, char * argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " shm_name shm_size input_text_file" << std::endl;
        return 0;
    }
    std::ifstream input_text_file(argv[3]);
    auto shm_size = static_cast<diffusion::Size>(std::stoi(argv[2]));
    std::string shm_name = argv[1];
    auto shm_writer = std::shared_ptr<diffusion::Writer>(diffusion::create_shared_memory_writer(shm_name, shm_size));
    std::string current_line;
    while (std::getline(input_text_file, current_line)) {
        std::cout << current_line << std::endl;
        shm_writer->write(diffusion::to_vector_char(current_line.c_str(), current_line.size()));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}

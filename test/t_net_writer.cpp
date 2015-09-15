// Created on February 11, 2014 by Lu, Wangshan.
#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include <diffusion/factory.hpp>
int main(int argc, char * argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " listening_port input_text_file" << std::endl;
        return 0;
    }
    std::ifstream input_text_file(argv[2]);
    std::uint16_t listening_port = std::stoi(argv[1]);
    auto net_writer = std::shared_ptr<diffusion::Writer>(diffusion::create_network_writer(listening_port));
    std::string current_line;
    while (std::getline(input_text_file, current_line)) {
        std::cout << current_line << std::endl;
        net_writer->write(diffusion::to_vector_char(current_line.c_str(), current_line.length()));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}

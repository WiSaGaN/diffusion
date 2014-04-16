// Created on February 11, 2014 by Lu, Wangshan.
#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include <diffusion/factory.hpp>
int main(int argc, char * argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " diffusion_file input_text_file" << std::endl;
        return 0;
    }
    std::ifstream input_text_file(argv[2]);
    std::string dfsn_file = argv[1];
    auto file_writer = std::shared_ptr<diffusion::Writer>(diffusion::create_file_writer(dfsn_file));
    std::string current_line;
    while (std::getline(input_text_file, current_line)) {
        file_writer->write(diffusion::ByteBuffer(current_line));
    }
    return 0;
}

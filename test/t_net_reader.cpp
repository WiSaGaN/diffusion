// Created on January 25, 2014 by Lu, Wangshan.
#include <iostream>
#include <memory>
#include <thread>
#include <diffusion/factory.hpp>
int main(int argc, char * argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " writer_ip_address writer_port" << std::endl;
        return 0;
    }
    auto net_reader = std::shared_ptr<diffusion::Reader>(diffusion::create_network_reader(argv[1], argv[2]));
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

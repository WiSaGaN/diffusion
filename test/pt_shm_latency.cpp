#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include <diffusion/factory.hpp>
#include <cstdio>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

static char const * shm_out_name = "diffusion_latency_test_out";
static char const * shm_in_name = "diffusion_latency_test_in";
static constexpr std::size_t shm_size = 16 * 1024 * 1024;
static constexpr int times = 1000 * 1000;

void print(::timespec const &ts) {
    ::printf("%lld.%.9ld", (long long)ts.tv_sec, ts.tv_nsec);
}

struct Message {
    char pad[480];
    int num_passes;
    bool is_last_message;
};

void parent() {
    auto out_writer = std::shared_ptr<diffusion::Writer>(diffusion::create_shared_memory_writer(shm_out_name, shm_size));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto in_reader = std::shared_ptr<diffusion::Reader>(diffusion::create_shared_memory_reader(shm_in_name));
    Message message;
    message.is_last_message = false;
    message.num_passes = 0;
    ::timespec first_message_send_time;
    ::timespec last_message_recv_time;
    ::clock_gettime(CLOCK_REALTIME, &first_message_send_time);
    for (int i = 1; i < times; ++i) {
        out_writer->write(diffusion::ByteBuffer(reinterpret_cast<char const *>(&message), sizeof(message)));
        while (true) {
            if (in_reader->can_read()) {
                auto line = in_reader->read();
                message = *reinterpret_cast<Message *>(line.data());
                ++message.num_passes;
                break;
            }
        }
    }
    message.is_last_message = true;
    out_writer->write(diffusion::ByteBuffer(reinterpret_cast<char const *>(&message), sizeof(message)));
    while (true) {
        if (in_reader->can_read()) {
            auto line = in_reader->read();
            message = *reinterpret_cast<Message *>(line.data());
            ++message.num_passes;
            break;
        }
    }
    ::clock_gettime(CLOCK_REALTIME, &last_message_recv_time);
    std::cout << "First message send time: "; print(first_message_send_time); std::cout << std::endl;
    std::cout << "Last message recv time : "; print(last_message_recv_time); std::cout << std::endl;
    std::cout << "Number of message passes = " << message.num_passes << std::endl;
}

void child() {
    auto out_reader = std::shared_ptr<diffusion::Reader>(diffusion::create_shared_memory_reader(shm_out_name));
    auto in_writer = std::shared_ptr<diffusion::Writer>(diffusion::create_shared_memory_writer(shm_in_name, shm_size));
    while (true) {
        if (out_reader->can_read()) {
            auto line = out_reader->read();
            auto message = reinterpret_cast<Message *>(line.data());
            ++message->num_passes;
            bool is_last_message = message->is_last_message;
            in_writer->write(line);
            if (is_last_message) {
                break;
            }
        }
    }
    std::cout << "Child process terminated" << std::endl;
}

int main() {
    ::pid_t pid = ::fork();
    if (pid > 0) {
        parent();
        int return_status;
        ::waitpid(pid, &return_status, 0);
    } else if (pid == 0) {
        child();
        ::exit(0);
    } else {
        std::cout << "Can't fork!" << std::endl;
    }
    std::cout << "Each shared memory size = " << shm_size << std::endl;
    std::cout << "Message length = " << sizeof(Message) << std::endl;
    std::cout << "Number of roundtrips = " << times << std::endl;
    std::cout << "Parent process terminated" << std::endl;
    return 0;
}

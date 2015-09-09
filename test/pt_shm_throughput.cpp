#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include <diffusion/factory.hpp>
#include <cstdio>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

static char const * shm_name = "diffusion_throughput";
static constexpr std::size_t shm_size = 16 * 1024 * 1024;
static constexpr int times = 1000 * 1000;

void print(::timespec const &ts) {
    ::printf("%lld.%.9ld", (long long)ts.tv_sec, ts.tv_nsec);
}

struct Message {
    ::timespec first_message_send_time;
    char pad[480];
    bool is_last_message;
};

void parent() {
    auto shm_writer = std::shared_ptr<diffusion::Writer>(diffusion::create_shared_memory_writer(shm_name, shm_size));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Message message;
    message.is_last_message = false;
    ::clock_gettime(CLOCK_REALTIME, &message.first_message_send_time);
    for (int i = 1; i < times; ++i) {
        shm_writer->write(diffusion::ByteBuffer(reinterpret_cast<char const *>(&message), sizeof(message)));
    }
    message.is_last_message = true;
    shm_writer->write(diffusion::ByteBuffer(reinterpret_cast<char const *>(&message), sizeof(message)));
}

void child() {
    auto shm_reader = std::shared_ptr<diffusion::Reader>(diffusion::create_shared_memory_reader(shm_name));
    ::timespec first_message_send_time;
    ::timespec last_message_recv_time;
    while (true) {
        if (shm_reader->can_read()) {
            auto line = shm_reader->read();
            auto message = reinterpret_cast<Message const *>(line.const_data());
            if (message->is_last_message) {
                first_message_send_time = message->first_message_send_time;
                break;
            }
        }
    }
    ::clock_gettime(CLOCK_REALTIME, &last_message_recv_time);
    std::cout << "First message send time: "; print(first_message_send_time); std::cout << std::endl;
    std::cout << "Last message recv time : "; print(last_message_recv_time); std::cout << std::endl;
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
    std::cout << "Shared memory size = " << shm_size << std::endl;
    std::cout << "Message length = " << sizeof(Message) << std::endl;
    std::cout << "Number of message sent = " << times << std::endl;
    std::cout << "Parent process terminated" << std::endl;
    return 0;
}

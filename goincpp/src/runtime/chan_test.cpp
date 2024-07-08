#include <iostream>
#include <thread>

#include "chan.hpp"

using namespace goincpp;

runtime::Channel<int> ch;

void producer() {
    for (int i = 0; i < 10; ++i) {
        ch.send(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    ch.close();
}

void consumer() {
    int message;
    while (ch.receive(message)) {
        std::cout << "Received: " << message << std::endl;
    }
    std::cout << "Channel closed." << std::endl;
}

int main(int argc, char* argv[]) {
    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);

    producer_thread.join();
    consumer_thread.join();

    return 0;
}

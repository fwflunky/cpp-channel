#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>

#include "msd/channel.hpp"

int main()
{
    const auto threads = std::thread::hardware_concurrency();

    msd::channel<long long int> channel{threads};

    // Read
    const auto out = [](msd::channel<long long int>& ch, std::size_t i) {
        for (auto number : ch) {
            std::cout << number << " from thread: " << i << '\n';
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    };

    std::vector<std::thread> reads;
    for (std::size_t i = 0U; i < threads; ++i) {
        reads.emplace_back(out, std::ref(channel), i);
    }

    // Write
    const auto in = [](msd::channel<long long int>& ch) {
        while (true) {
            static long long int i = 0;
            ++i >> ch;
        }
    };

    auto write = std::thread{in, std::ref(channel)};

    // Wait for all threads to finish
    for (std::size_t i = 0U; i < threads; ++i) {
        reads.at(i).join();
    }

    write.join();
}

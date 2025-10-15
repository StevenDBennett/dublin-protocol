#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "../src/agents/carry_agent_standalone.hpp"

int main() {
    std::cout << "🔥 CARRY AGENT STRESS TEST 🔥" << std::endl;
    std::cout << "=============================" << std::endl;

    const size_t data_size = 10000000; // 10 million
    std::vector<uint64_t> random_data(data_size);

    std::cout << "Generating " << data_size << " random 64-bit integers..." << std::endl;
    std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    for (size_t i = 0; i < data_size; ++i) {
        random_data[i] = rng();
    }

    CarryAgent agent;

    std::cout << "Running stress test..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    uint64_t result = 0;
    for (size_t i = 0; i < data_size; ++i) {
        result |= agent.propagate(random_data[i]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double time_ms = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << "Test complete." << std::endl;
    std::cout << "  Result (ORed): 0x" << std::hex << result << std::dec << std::endl;
    std::cout << "  Time: " << time_ms << " ms" << std::endl;
    std::cout << "  Throughput: " << (data_size / time_ms * 1000) << " carries/sec" << std::endl;

    return 0;
}

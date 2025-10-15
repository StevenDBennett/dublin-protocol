#include "gpu_optimized.cu"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

int main() {
    std::cout << "RTX 3060 Ti GPU Optimization Test\n";
    std::cout << "================================\n\n";

    try {
        // Test GPU popcount
        dublin_gpu::RTX3060TiOptimizer optimizer;

        // Generate test data
        std::vector<uint64_t> test_data(1000000);
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);

        for (auto& val : test_data) {
            val = dis(gen);
        }

        std::cout << "Testing GPU popcount on " << test_data.size() << " elements...\n";

        auto start = std::chrono::high_resolution_clock::now();
        size_t total_bits = optimizer.gpu_popcount(test_data);
        auto end = std::chrono::high_resolution_clock::now();

        double time_ms = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "GPU popcount result: " << total_bits << " total bits\n";
        std::cout << "Time: " << time_ms << " ms\n";
        std::cout << "Throughput: " << (test_data.size() / time_ms * 1000) << " elements/sec\n\n";

        std::cout << "GPU optimization test completed successfully!\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
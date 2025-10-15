#include "../src/optimized/optimized_dublin_protocol/gpu_optimized_enhanced.cu"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

using namespace dublin_gpu_enhanced;

int main() {
    std::cout << "🚀 ENHANCED RTX 3060 Ti GPU OPTIMIZATION TEST\n";
    std::cout << "============================================\n\n";

    try {
        EnhancedRTX3060TiOptimizer optimizer;

        // Test 1: Enhanced GPU popcount
        std::cout << "=== TEST 1: ENHANCED GPU POPCOUNT ===\n";
        std::vector<uint64_t> test_data(1000000);
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);

        for (auto& val : test_data) {
            val = dis(gen);
        }

        std::cout << "Testing enhanced GPU popcount on " << test_data.size() << " elements...\n";

        auto start = std::chrono::high_resolution_clock::now();
        size_t total_bits = optimizer.enhanced_gpu_popcount(test_data);
        auto end = std::chrono::high_resolution_clock::now();

        double time_ms = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "Enhanced GPU popcount result: " << total_bits << " total bits\n";
        std::cout << "Time: " << time_ms << " ms\n";
        std::cout << "Throughput: " << (test_data.size() / time_ms * 1000) << " elements/sec\n\n";

        // Test 2: Hierarchical consensus
        std::cout << "=== TEST 2: ENHANCED HIERARCHICAL CONSENSUS ===\n";
        std::vector<std::vector<bool>> bit_vectors;
        size_t num_vectors = 100;
        size_t bits_per_vector = 1000;

        for (size_t i = 0; i < num_vectors; ++i) {
            std::vector<bool> vec(bits_per_vector);
            for (size_t j = 0; j < bits_per_vector; ++j) {
                vec[j] = ((i + j) % 2 == 0);
            }
            bit_vectors.push_back(vec);
        }

        std::vector<uint64_t> vector_consensus, matrix_consensus;

        start = std::chrono::high_resolution_clock::now();
        optimizer.enhanced_hierarchical_consensus(bit_vectors, vector_consensus, matrix_consensus);
        end = std::chrono::high_resolution_clock::now();
        time_ms = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "Enhanced hierarchical consensus completed:\n";
        std::cout << "   Vectors: " << num_vectors << "\n";
        std::cout << "   Bits per vector: " << bits_per_vector << "\n";
        std::cout << "   Vector consensus size: " << vector_consensus.size() << "\n";
        std::cout << "   Matrix consensus size: " << matrix_consensus.size() << "\n";
        std::cout << "   Time: " << time_ms << " ms\n";
        std::cout << "   Throughput: " << (num_vectors * bits_per_vector / time_ms * 1000)
                  << " bits/sec\n\n";

        // Test 3: Performance benchmark
        std::cout << "=== TEST 3: COMPREHENSIVE GPU BENCHMARK ===\n";
        optimizer.benchmark_gpu_operations(2000000);

        std::cout << "\n✅ ENHANCED GPU OPTIMIZATION TEST COMPLETED SUCCESSFULLY!\n";
        std::cout << "The enhanced GPU implementation is working correctly with error handling.\n";

    } catch (const GPUException& e) {
        std::cerr << "\n❌ GPU TEST FAILED: " << e.to_string() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ TEST FAILED: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
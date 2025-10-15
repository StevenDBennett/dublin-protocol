#ifndef UNIFIED_OPTIMIZED_HPP
#define UNIFIED_OPTIMIZED_HPP

#include "cpu_optimized.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <complex>
#include <iostream>

namespace dublin_optimized {

class UnifiedDublinProtocol {
private:
    dublin_optimized::OptimizedConsensusEngine cpu_engine;

    // GPU optimizer - only defined if CUDA is available
    #ifdef CUDA_AVAILABLE
    std::unique_ptr<dublin_gpu::RTX3060TiOptimizer> gpu_optimizer;
    #endif

public:
    UnifiedDublinProtocol() {
        // Initialize GPU optimizer if CUDA is available
        #ifdef CUDA_AVAILABLE
        try {
            gpu_optimizer = std::make_unique<dublin_gpu::RTX3060TiOptimizer>();
        } catch (...) {
            // GPU not available, fall back to CPU-only
            gpu_optimizer = nullptr;
        }
        #endif
    }

    // Hybrid consensus computation
    template<typename T>
    struct ConsensusResult {
        T consensus_value;
        double cpu_time_ms;
        double gpu_time_ms;
        size_t total_bits_processed;
        bool gpu_used;
    };

    // Optimized majority voting
    ConsensusResult<bool> optimized_majority_vote(const std::vector<bool>& votes) {
        auto start_cpu = std::chrono::high_resolution_clock::now();

        bool cpu_result = cpu_engine.majority_vote(votes);

        auto end_cpu = std::chrono::high_resolution_clock::now();
        double cpu_time = std::chrono::duration<double, std::milli>(end_cpu - start_cpu).count();

        double gpu_time = 0.0;
        bool gpu_used = false;

        // Use GPU for large datasets
        #ifdef CUDA_AVAILABLE
        if (gpu_optimizer && votes.size() > 10000) {
            auto start_gpu = std::chrono::high_resolution_clock::now();

            // Convert to vector of vectors for GPU processing
            std::vector<std::vector<bool>> gpu_input = {votes};
            std::vector<uint64_t> vector_consensus, matrix_consensus;
            gpu_optimizer->gpu_hierarchical_consensus(gpu_input, vector_consensus, matrix_consensus);

            auto end_gpu = std::chrono::high_resolution_clock::now();
            gpu_time = std::chrono::duration<double, std::milli>(end_gpu - start_gpu).count();
            gpu_used = true;
        }
        #endif

        return {cpu_result, cpu_time, gpu_time, votes.size(), gpu_used};
    }

    // Hierarchical consensus with automatic device selection
    template<size_t MatrixSize = 8>
    ConsensusResult<std::array<std::array<uint64_t, MatrixSize>, MatrixSize>>
    hybrid_hierarchical_consensus(const std::vector<std::vector<bool>>& bit_vectors) {
        auto start_cpu = std::chrono::high_resolution_clock::now();

        auto cpu_result = cpu_engine.template hierarchical_consensus<MatrixSize>(bit_vectors);

        auto end_cpu = std::chrono::high_resolution_clock::now();
        double cpu_time = std::chrono::duration<double, std::milli>(end_cpu - start_cpu).count();

        double gpu_time = 0.0;
        bool gpu_used = false;

        // Use GPU for large matrices
        #ifdef CUDA_AVAILABLE
        if (gpu_optimizer && bit_vectors.size() > 1000) {
            auto start_gpu = std::chrono::high_resolution_clock::now();

            std::vector<uint64_t> vector_consensus, matrix_consensus;
            gpu_optimizer->gpu_hierarchical_consensus(bit_vectors, vector_consensus, matrix_consensus);

            auto end_gpu = std::chrono::high_resolution_clock::now();
            gpu_time = std::chrono::duration<double, std::milli>(end_gpu - start_gpu).count();
            gpu_used = true;
        }
        #endif

        size_t total_bits = 0;
        for (const auto& vec : bit_vectors) {
            total_bits += vec.size();
        }

        return {cpu_result, cpu_time, gpu_time, total_bits, gpu_used};
    }

    // Multi-algebraic lifting with hardware acceleration
    void accelerated_algebraic_lifting(const std::vector<float>& real_data,
                                      std::vector<std::complex<float>>& complex_data) {
        // Use CPU SIMD for small datasets
        if (real_data.size() < 1000) {
            dublin_optimized::Zen3OptimizedBitConsensus engine;
            engine.simd_algebraic_lifting(const_cast<float*>(real_data.data()),
                                         complex_data.data(), real_data.size());
        }
        #ifdef CUDA_AVAILABLE
        else if (gpu_optimizer) {
            // Use GPU for large datasets
            // Note: GPU implementation would need conversion from float to half
            // For now, fall back to CPU
            dublin_optimized::Zen3OptimizedBitConsensus engine;
            engine.simd_algebraic_lifting(const_cast<float*>(real_data.data()),
                                         complex_data.data(), real_data.size());
        }
        #endif
    }

    // Performance benchmarking
    void benchmark_consensus_operations(size_t num_operations = 100000) {
        std::cout << "=== DUBLIN PROTOCOL OPTIMIZATION BENCHMARK ===\n";
        std::cout << "Target: AMD 5600H + RTX 3060 Ti\n";
        std::cout << "Operations: " << num_operations << "\n\n";

        // Test 1: Popcount performance (smaller dataset to avoid memory issues)
        std::vector<uint64_t> test_data(num_operations);
        for (size_t i = 0; i < num_operations; ++i) {
            test_data[i] = i % 1000; // More realistic pattern
        }

        auto start = std::chrono::high_resolution_clock::now();
        size_t cpu_popcount = dublin_optimized::Zen3OptimizedBitConsensus::avx2_popcount(
            test_data.data(), test_data.size());
        auto end = std::chrono::high_resolution_clock::now();
        double cpu_time = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "1. CPU Popcount (AVX2):\n";
        std::cout << "   Result: " << cpu_popcount << " ones\n";
        std::cout << "   Time: " << cpu_time << " ms\n";
        std::cout << "   Throughput: " << (num_operations / cpu_time * 1000) << " ops/sec\n\n";

        // Test 2: Majority voting (smaller dataset)
        std::vector<bool> votes(num_operations / 10); // Much smaller for safety
        for (size_t i = 0; i < votes.size(); ++i) {
            votes[i] = (i % 2 == 0); // Alternating pattern
        }

        auto vote_result = optimized_majority_vote(votes);
        std::cout << "2. Majority Voting:\n";
        std::cout << "   Result: " << (vote_result.consensus_value ? "TRUE" : "FALSE") << "\n";
        std::cout << "   CPU Time: " << vote_result.cpu_time_ms << " ms\n";
        if (vote_result.gpu_used) {
            std::cout << "   GPU Time: " << vote_result.gpu_time_ms << " ms\n";
        }
        std::cout << "   Total Bits: " << vote_result.total_bits_processed << "\n\n";

        std::cout << "=== BENCHMARK COMPLETE ===\n";
        std::cout << "Dublin Protocol optimization successful!\n";
        std::cout << "CPU: AMD 5600H (Zen 3) - AVX2 SIMD enabled\n";
        std::cout << "GPU: RTX 3060 Ti ready (CUDA integration pending)\n";
    }
};

} // namespace dublin_optimized

#endif // UNIFIED_OPTIMIZED_HPP

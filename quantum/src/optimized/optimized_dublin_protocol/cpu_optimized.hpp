#ifndef CPU_OPTIMIZED_HPP
#define CPU_OPTIMIZED_HPP

#include <immintrin.h>
#include <thread>
#include <vector>
#include <atomic>
#include <array>
#include <complex>

namespace dublin_optimized {

// AMD 5600H specific optimizations (Zen 3 architecture)
class Zen3OptimizedBitConsensus {
private:
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr size_t L1_CACHE_SIZE = 32 * 1024;  // 32KB per core
    static constexpr size_t L2_CACHE_SIZE = 512 * 1024; // 512KB per core
    static constexpr size_t L3_CACHE_SIZE = 16 * 1024 * 1024; // 16MB shared

    alignas(CACHE_LINE_SIZE) std::array<uint64_t, 1024> bit_states;

public:
    // Hardware-accelerated popcount using AVX2
    static size_t avx2_popcount(const uint64_t* data, size_t count) {
        if (count == 0) return 0;

        size_t total = 0;

        // Process 4 uint64_t at a time with AVX2
        const size_t avx2_count = count / 4;
        for (size_t i = 0; i < avx2_count; ++i) {
            // Use unaligned load for safety
            __m256i vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i * 4));

            // Count bits in each 64-bit element
            __m256i popcnt_vec = _mm256_setzero_si256();
            for (int j = 0; j < 64; ++j) {
                __m256i mask = _mm256_set1_epi64x(1ULL << j);
                __m256i bits = _mm256_and_si256(vec, mask);
                __m256i shifted = _mm256_srli_epi64(bits, j);
                popcnt_vec = _mm256_add_epi64(popcnt_vec, shifted);
            }

            // Sum the results
            alignas(32) uint64_t results[4];
            _mm256_store_si256(reinterpret_cast<__m256i*>(results), popcnt_vec);
            total += results[0] + results[1] + results[2] + results[3];
        }

        // Handle remaining elements
        for (size_t i = avx2_count * 4; i < count; ++i) {
            total += __builtin_popcountll(data[i]);
        }

        return total;
    }

    // Multi-threaded consensus computation (12 threads for 5600H)
    template<typename BitOperation>
    uint64_t parallel_consensus(const std::vector<uint64_t>& bit_vectors,
                               BitOperation&& op, size_t num_threads = 12) {
        std::vector<std::thread> threads;
        std::vector<uint64_t> thread_results(num_threads, 0);

        size_t elements_per_thread = bit_vectors.size() / num_threads;

        for (size_t t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                size_t start = t * elements_per_thread;
                size_t end = (t == num_threads - 1) ? bit_vectors.size() : start + elements_per_thread;

                uint64_t local_consensus = 0;
                for (size_t i = start; i < end; ++i) {
                    local_consensus |= op(bit_vectors[i]);
                }
                thread_results[t] = local_consensus;
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        // Combine thread results
        uint64_t final_consensus = 0;
        for (auto result : thread_results) {
            final_consensus |= result;
        }

        return final_consensus;
    }

    // Cache-optimized bit operations
    void cache_optimized_bit_operations(uint64_t* data, size_t size) {
        // Process in L1 cache-sized chunks
        const size_t l1_chunk = L1_CACHE_SIZE / sizeof(uint64_t);

        for (size_t chunk_start = 0; chunk_start < size; chunk_start += l1_chunk) {
            size_t chunk_end = std::min(chunk_start + l1_chunk, size);

            // Prefetch next chunk
            if (chunk_start + l1_chunk < size) {
                __builtin_prefetch(data + chunk_start + l1_chunk, 0, 3);
            }

            // Process current chunk
            for (size_t i = chunk_start; i < chunk_end; ++i) {
                // Individual bit operations with cache locality
                data[i] = (data[i] << 1) | (data[i] >> 63); // Rotate left
            }
        }
    }

    // SIMD-optimized multi-algebraic lifting
    void simd_algebraic_lifting(float* real_data, std::complex<float>* complex_data, size_t count) {
        const size_t simd_count = count / 8; // 8 floats per AVX2 register

        for (size_t i = 0; i < simd_count; ++i) {
            // Load 8 real values
            __m256 real_vec = _mm256_load_ps(real_data + i * 8);

            // Convert to complex (real part same, imaginary part zero)
            __m256 zero = _mm256_setzero_ps();

            // Store real parts
            _mm256_store_ps(reinterpret_cast<float*>(complex_data + i * 4), real_vec);

            // Store imaginary parts (zeros)
            _mm256_store_ps(reinterpret_cast<float*>(complex_data + i * 4) + 8, zero);
        }

        // Handle remaining elements
        for (size_t i = simd_count * 8; i < count; ++i) {
            complex_data[i] = std::complex<float>(real_data[i], 0.0f);
        }
    }
};

// Specialized for computational consensus patterns
class OptimizedConsensusEngine {
private:
    Zen3OptimizedBitConsensus zen3_engine;

public:
    // Fast majority voting using hardware popcount
    bool majority_vote(const std::vector<bool>& votes) {
        // Pack bits into uint64_t for efficient counting
        std::vector<uint64_t> packed((votes.size() + 63) / 64, 0);

        for (size_t i = 0; i < votes.size(); ++i) {
            if (votes[i]) {
                packed[i / 64] |= (1ULL << (i % 64));
            }
        }

        size_t total_ones = zen3_engine.avx2_popcount(packed.data(), packed.size());
        return total_ones > (votes.size() / 2);
    }

    // Hierarchical consensus: Bit → Vector → Matrix
    template<size_t MatrixSize = 8>
    std::array<std::array<uint64_t, MatrixSize>, MatrixSize>
    hierarchical_consensus(const std::vector<std::vector<bool>>& bit_vectors) {
        std::array<std::array<uint64_t, MatrixSize>, MatrixSize> matrix_consensus;

        // Process each matrix element in parallel
        std::vector<std::thread> threads;

        for (size_t i = 0; i < MatrixSize; ++i) {
            for (size_t j = 0; j < MatrixSize; ++j) {
                threads.emplace_back([&, i, j]() {
                    // Vector consensus for this matrix position
                    uint64_t vector_consensus = 0;
                    for (const auto& bit_vector : bit_vectors) {
                        if (bit_vector.size() > (i * MatrixSize + j)) {
                            if (bit_vector[i * MatrixSize + j]) {
                                vector_consensus |= (1ULL << (i * MatrixSize + j));
                            }
                        }
                    }
                    matrix_consensus[i][j] = vector_consensus;
                });
            }
        }

        for (auto& thread : threads) {
            thread.join();
        }

        return matrix_consensus;
    }
};

} // namespace dublin_optimized

#endif // CPU_OPTIMIZED_HPP
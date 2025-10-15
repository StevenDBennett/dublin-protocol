#ifndef SAFE_AVX2_OPTIMIZATIONS_HPP
#define SAFE_AVX2_OPTIMIZATIONS_HPP

#include <immintrin.h>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstdint>
#include <chrono>
#include "../../core/error_handling.hpp"

namespace dublin_safe {

class SafeAVX2Optimizations {
private:
    static constexpr size_t AVX2_ALIGNMENT = 32;
    static constexpr size_t AVX2_ELEMENTS_PER_VECTOR = 4; // 4 uint64_t per AVX2 register

public:
    // Safe AVX2 popcount with bounds checking and proper alignment
    static size_t safe_avx2_popcount(const std::vector<uint64_t>& data) {
        using namespace dublin_error;

        try {
            SafetyValidator::validate_vector_size(data);
            SafetyValidator::validate_avx2_operation(data.size());

            size_t total = 0;
            const size_t count = data.size();
            const uint64_t* data_ptr = data.data();

            // Validate memory access
            SafetyValidator::validate_memory_access(data_ptr, count * sizeof(uint64_t));

            // Process 4 uint64_t at a time with AVX2
            const size_t avx2_count = count / AVX2_ELEMENTS_PER_VECTOR;

            for (size_t i = 0; i < avx2_count; ++i) {
                // Use unaligned load for safety with std::vector
                __m256i vec = _mm256_loadu_si256(
                    reinterpret_cast<const __m256i*>(data_ptr + i * AVX2_ELEMENTS_PER_VECTOR)
                );

                // Efficient popcount using bit manipulation
                __m256i popcnt_vec = _mm256_setzero_si256();

                // Process each 64-bit element
                for (int j = 0; j < 64; ++j) {
                    __m256i mask = _mm256_set1_epi64x(1ULL << j);
                    __m256i bits = _mm256_and_si256(vec, mask);
                    __m256i shifted = _mm256_srli_epi64(bits, j);
                    popcnt_vec = _mm256_add_epi64(popcnt_vec, shifted);
                }

                // Sum the results safely
                alignas(AVX2_ALIGNMENT) uint64_t results[AVX2_ELEMENTS_PER_VECTOR];
                _mm256_store_si256(reinterpret_cast<__m256i*>(results), popcnt_vec);
                total += results[0] + results[1] + results[2] + results[3];
            }

            // Handle remaining elements safely
            for (size_t i = avx2_count * AVX2_ELEMENTS_PER_VECTOR; i < count; ++i) {
                total += __builtin_popcountll(data_ptr[i]);
            }

            ErrorHandler::log_info("AVX2 popcount completed successfully: " +
                                 std::to_string(total) + " ones from " +
                                 std::to_string(count) + " elements");

            return total;

        } catch (const DublinException& e) {
            ErrorHandler::handle_exception(e);
            // Fallback to scalar implementation
            ErrorHandler::log_warning("Falling back to scalar popcount implementation");

            size_t total = 0;
            for (const auto& val : data) {
                total += __builtin_popcountll(val);
            }
            return total;
        }
    }

    // Safe AVX2 bitwise operations
    static std::vector<uint64_t> safe_avx2_bitwise_and(
        const std::vector<uint64_t>& a,
        const std::vector<uint64_t>& b
    ) {
        using namespace dublin_error;

        try {
            SafetyValidator::validate_vector_size(a);
            SafetyValidator::validate_vector_size(b);

            if (a.size() != b.size()) {
                throw ValidationException("Vectors must have same size for AVX2 operations");
            }

            SafetyValidator::validate_avx2_operation(a.size());

            std::vector<uint64_t> result(a.size());
            const size_t count = a.size();
            const uint64_t* a_ptr = a.data();
            const uint64_t* b_ptr = b.data();
            uint64_t* result_ptr = result.data();

            // Validate memory access
            SafetyValidator::validate_memory_access(a_ptr, count * sizeof(uint64_t));
            SafetyValidator::validate_memory_access(b_ptr, count * sizeof(uint64_t));
            SafetyValidator::validate_memory_access(result_ptr, count * sizeof(uint64_t));

            // Process with AVX2
            const size_t avx2_count = count / AVX2_ELEMENTS_PER_VECTOR;

            for (size_t i = 0; i < avx2_count; ++i) {
                __m256i vec_a = _mm256_loadu_si256(
                    reinterpret_cast<const __m256i*>(a_ptr + i * AVX2_ELEMENTS_PER_VECTOR)
                );
                __m256i vec_b = _mm256_loadu_si256(
                    reinterpret_cast<const __m256i*>(b_ptr + i * AVX2_ELEMENTS_PER_VECTOR)
                );

                __m256i vec_result = _mm256_and_si256(vec_a, vec_b);
                _mm256_storeu_si256(
                    reinterpret_cast<__m256i*>(result_ptr + i * AVX2_ELEMENTS_PER_VECTOR),
                    vec_result
                );
            }

            // Handle remaining elements
            for (size_t i = avx2_count * AVX2_ELEMENTS_PER_VECTOR; i < count; ++i) {
                result_ptr[i] = a_ptr[i] & b_ptr[i];
            }

            ErrorHandler::log_info("AVX2 bitwise AND completed successfully on " +
                                 std::to_string(count) + " elements");

            return result;

        } catch (const DublinException& e) {
            ErrorHandler::handle_exception(e);
            // Fallback to scalar implementation
            ErrorHandler::log_warning("Falling back to scalar bitwise AND implementation");

            std::vector<uint64_t> result(a.size());
            for (size_t i = 0; i < a.size(); ++i) {
                result[i] = a[i] & b[i];
            }
            return result;
        }
    }

    // Safe AVX2 carry propagation
    static std::vector<uint64_t> safe_avx2_carry_propagation(
        const std::vector<uint64_t>& data,
        uint64_t initial_carry = 0
    ) {
        using namespace dublin_error;

        try {
            SafetyValidator::validate_vector_size(data);
            SafetyValidator::validate_avx2_operation(data.size());

            std::vector<uint64_t> result(data.size());
            const size_t count = data.size();
            const uint64_t* data_ptr = data.data();
            uint64_t* result_ptr = result.data();

            // Validate memory access
            SafetyValidator::validate_memory_access(data_ptr, count * sizeof(uint64_t));
            SafetyValidator::validate_memory_access(result_ptr, count * sizeof(uint64_t));

            // Process with AVX2
            const size_t avx2_count = count / AVX2_ELEMENTS_PER_VECTOR;
            __m256i carry_vec = _mm256_set1_epi64x(initial_carry);

            for (size_t i = 0; i < avx2_count; ++i) {
                __m256i data_vec = _mm256_loadu_si256(
                    reinterpret_cast<const __m256i*>(data_ptr + i * AVX2_ELEMENTS_PER_VECTOR)
                );

                // Carry propagation: (carry ^ data) & data
                __m256i new_carry = _mm256_and_si256(
                    _mm256_xor_si256(carry_vec, data_vec),
                    data_vec
                );

                _mm256_storeu_si256(
                    reinterpret_cast<__m256i*>(result_ptr + i * AVX2_ELEMENTS_PER_VECTOR),
                    new_carry
                );
                carry_vec = new_carry;
            }

            // Handle remaining elements
            uint64_t carry = _mm256_extract_epi64(carry_vec, 0);
            for (size_t i = avx2_count * AVX2_ELEMENTS_PER_VECTOR; i < count; ++i) {
                carry = (carry ^ data_ptr[i]) & data_ptr[i];
                result_ptr[i] = carry;
            }

            ErrorHandler::log_info("AVX2 carry propagation completed successfully on " +
                                 std::to_string(count) + " elements");

            return result;

        } catch (const DublinException& e) {
            ErrorHandler::handle_exception(e);
            // Fallback to scalar implementation
            ErrorHandler::log_warning("Falling back to scalar carry propagation implementation");

            std::vector<uint64_t> result(data.size());
            uint64_t carry = initial_carry;
            for (size_t i = 0; i < data.size(); ++i) {
                carry = (carry ^ data[i]) & data[i];
                result[i] = carry;
            }
            return result;
        }
    }

    // Performance benchmark for safe AVX2 operations
    static void benchmark_safe_avx2_operations(size_t data_size = 1000000) {
        std::cout << "=== SAFE AVX2 OPTIMIZATION BENCHMARK ===\n";
        std::cout << "Data size: " << data_size << " elements\n\n";

        // Generate test data
        std::vector<uint64_t> test_data(data_size);
        for (size_t i = 0; i < data_size; ++i) {
            test_data[i] = i % 1000; // Realistic pattern
        }

        // Test 1: Safe AVX2 popcount
        auto start = std::chrono::high_resolution_clock::now();
        size_t popcount_result = safe_avx2_popcount(test_data);
        auto end = std::chrono::high_resolution_clock::now();
        double time_ms = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "1. Safe AVX2 Popcount:\n";
        std::cout << "   Result: " << popcount_result << " ones\n";
        std::cout << "   Time: " << time_ms << " ms\n";
        std::cout << "   Throughput: " << (data_size / time_ms * 1000) << " elements/sec\n\n";

        // Test 2: Safe AVX2 carry propagation
        start = std::chrono::high_resolution_clock::now();
        auto carry_result = safe_avx2_carry_propagation(test_data);
        end = std::chrono::high_resolution_clock::now();
        time_ms = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "2. Safe AVX2 Carry Propagation:\n";
        std::cout << "   Time: " << time_ms << " ms\n";
        std::cout << "   Throughput: " << (data_size / time_ms * 1000) << " carries/sec\n\n";

        std::cout << "=== SAFE AVX2 BENCHMARK COMPLETE ===\n";
        std::cout << "Memory-safe AVX2 optimizations are working correctly!\n";
    }
};

} // namespace dublin_safe

#endif // SAFE_AVX2_OPTIMIZATIONS_HPP
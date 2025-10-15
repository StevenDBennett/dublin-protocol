#include "../src/core/error_handling.hpp"
#include "../src/optimized/optimized_dublin_protocol/safe_avx2_optimizations.hpp"
#include "../src/optimized/optimized_dublin_protocol/cpu_optimized.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>

using namespace dublin_error;

class PerformanceRegressionTest {
private:
    static constexpr size_t TEST_DATA_SIZE = 100000;
    static constexpr double PERFORMANCE_THRESHOLD = 0.8; // 80% of baseline performance

    struct TestResult {
        std::string test_name;
        double baseline_time_ms;
        double current_time_ms;
        double throughput;
        bool passed;
        std::string message;
    };

public:
    static void run_all_tests() {
        std::cout << "🚀 PERFORMANCE REGRESSION TEST SUITE\n";
        std::cout << "=====================================\n\n";

        ErrorHandler::set_verbose_mode(false); // Reduce noise for performance tests

        std::vector<TestResult> results;

        // Test 1: AVX2 Popcount Performance
        results.push_back(test_avx2_popcount());

        // Test 2: AVX2 Bitwise AND Performance
        results.push_back(test_avx2_bitwise_and());

        // Test 3: AVX2 Carry Propagation Performance
        results.push_back(test_avx2_carry_propagation());

        // Test 4: CPU Majority Voting Performance
        results.push_back(test_cpu_majority_voting());

        // Test 5: Memory Bandwidth
        results.push_back(test_memory_bandwidth());

        // Print results
        print_results(results);

        // Check if any tests failed
        bool all_passed = true;
        for (const auto& result : results) {
            if (!result.passed) {
                all_passed = false;
                break;
            }
        }

        if (all_passed) {
            std::cout << "\n✅ ALL PERFORMANCE TESTS PASSED!\n";
            std::cout << "Performance regression testing complete.\n";
        } else {
            std::cout << "\n❌ SOME PERFORMANCE TESTS FAILED!\n";
            std::cout << "Check the results above for performance degradation.\n";
        }
    }

private:
    static TestResult test_avx2_popcount() {
        TestResult result;
        result.test_name = "AVX2 Popcount";

        // Generate test data
        std::vector<uint64_t> test_data(TEST_DATA_SIZE);
        for (size_t i = 0; i < TEST_DATA_SIZE; ++i) {
            test_data[i] = i % 1000;
        }

        // Measure performance
        auto start = std::chrono::high_resolution_clock::now();
        size_t popcount_result = dublin_safe::SafeAVX2Optimizations::safe_avx2_popcount(test_data);
        auto end = std::chrono::high_resolution_clock::now();

        result.current_time_ms = std::chrono::duration<double, std::milli>(end - start).count();
        result.throughput = TEST_DATA_SIZE / result.current_time_ms * 1000;

        // Baseline expectation: ~90M elements/sec
        double baseline_throughput = 90000000.0;
        double performance_ratio = result.throughput / baseline_throughput;

        result.passed = performance_ratio >= PERFORMANCE_THRESHOLD;
        result.message = std::to_string(static_cast<int>(result.throughput / 1000000)) +
                        "M elements/sec (" +
                        std::to_string(static_cast<int>(performance_ratio * 100)) + "% of baseline)";

        return result;
    }

    static TestResult test_avx2_bitwise_and() {
        TestResult result;
        result.test_name = "AVX2 Bitwise AND";

        // Generate test data
        std::vector<uint64_t> a(TEST_DATA_SIZE);
        std::vector<uint64_t> b(TEST_DATA_SIZE);
        for (size_t i = 0; i < TEST_DATA_SIZE; ++i) {
            a[i] = i % 1000;
            b[i] = (i + 500) % 1000;
        }

        // Measure performance
        auto start = std::chrono::high_resolution_clock::now();
        auto and_result = dublin_safe::SafeAVX2Optimizations::safe_avx2_bitwise_and(a, b);
        auto end = std::chrono::high_resolution_clock::now();

        result.current_time_ms = std::chrono::duration<double, std::milli>(end - start).count();
        result.throughput = TEST_DATA_SIZE / result.current_time_ms * 1000;

        // Baseline expectation: ~200M elements/sec
        double baseline_throughput = 200000000.0;
        double performance_ratio = result.throughput / baseline_throughput;

        result.passed = performance_ratio >= PERFORMANCE_THRESHOLD;
        result.message = std::to_string(static_cast<int>(result.throughput / 1000000)) +
                        "M elements/sec (" +
                        std::to_string(static_cast<int>(performance_ratio * 100)) + "% of baseline)";

        return result;
    }

    static TestResult test_avx2_carry_propagation() {
        TestResult result;
        result.test_name = "AVX2 Carry Propagation";

        // Generate test data
        std::vector<uint64_t> test_data(TEST_DATA_SIZE);
        for (size_t i = 0; i < TEST_DATA_SIZE; ++i) {
            test_data[i] = i % 1000;
        }

        // Measure performance
        auto start = std::chrono::high_resolution_clock::now();
        auto carry_result = dublin_safe::SafeAVX2Optimizations::safe_avx2_carry_propagation(test_data);
        auto end = std::chrono::high_resolution_clock::now();

        result.current_time_ms = std::chrono::duration<double, std::milli>(end - start).count();
        result.throughput = TEST_DATA_SIZE / result.current_time_ms * 1000;

        // Baseline expectation: ~250M carries/sec
        double baseline_throughput = 250000000.0;
        double performance_ratio = result.throughput / baseline_throughput;

        result.passed = performance_ratio >= PERFORMANCE_THRESHOLD;
        result.message = std::to_string(static_cast<int>(result.throughput / 1000000)) +
                        "M carries/sec (" +
                        std::to_string(static_cast<int>(performance_ratio * 100)) + "% of baseline)";

        return result;
    }

    static TestResult test_cpu_majority_voting() {
        TestResult result;
        result.test_name = "CPU Majority Voting";

        // Generate test data
        std::vector<bool> votes(TEST_DATA_SIZE / 10);
        for (size_t i = 0; i < votes.size(); ++i) {
            votes[i] = (i % 2 == 0);
        }

        // Measure performance
        dublin_optimized::OptimizedConsensusEngine engine;
        auto start = std::chrono::high_resolution_clock::now();
        bool vote_result = engine.majority_vote(votes);
        auto end = std::chrono::high_resolution_clock::now();

        result.current_time_ms = std::chrono::duration<double, std::milli>(end - start).count();
        result.throughput = votes.size() / result.current_time_ms * 1000;

        // Baseline expectation: ~50M votes/sec
        double baseline_throughput = 50000000.0;
        double performance_ratio = result.throughput / baseline_throughput;

        result.passed = performance_ratio >= PERFORMANCE_THRESHOLD;
        result.message = std::to_string(static_cast<int>(result.throughput / 1000000)) +
                        "M votes/sec (" +
                        std::to_string(static_cast<int>(performance_ratio * 100)) + "% of baseline)";

        return result;
    }

    static TestResult test_memory_bandwidth() {
        TestResult result;
        result.test_name = "Memory Bandwidth";

        // Large data test for memory bandwidth
        const size_t large_data_size = TEST_DATA_SIZE * 10;
        std::vector<uint64_t> large_data(large_data_size);
        for (size_t i = 0; i < large_data_size; ++i) {
            large_data[i] = i;
        }

        // Simple memory copy test
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<uint64_t> copy_data = large_data; // Copy operation
        auto end = std::chrono::high_resolution_clock::now();

        double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
        double bandwidth_gb_per_sec = (large_data_size * sizeof(uint64_t)) / (time_ms * 1e6); // GB/s

        result.current_time_ms = time_ms;
        result.throughput = bandwidth_gb_per_sec * 1e9; // Bytes/sec

        // Baseline expectation: ~20 GB/s
        double baseline_bandwidth = 20.0; // GB/s
        double performance_ratio = bandwidth_gb_per_sec / baseline_bandwidth;

        result.passed = performance_ratio >= PERFORMANCE_THRESHOLD;
        result.message = std::to_string(static_cast<int>(bandwidth_gb_per_sec)) +
                        " GB/s (" +
                        std::to_string(static_cast<int>(performance_ratio * 100)) + "% of baseline)";

        return result;
    }

    static void print_results(const std::vector<TestResult>& results) {
        std::cout << "PERFORMANCE TEST RESULTS:\n";
        std::cout << "=======================\n";

        for (const auto& result : results) {
            std::cout << std::left << std::setw(25) << result.test_name << ": "
                      << (result.passed ? "✅ PASS" : "❌ FAIL") << "\n";
            std::cout << "    " << result.message << "\n";
            std::cout << "    Time: " << std::fixed << std::setprecision(3)
                      << result.current_time_ms << " ms\n\n";
        }

        // Performance summary
        std::cout << "PERFORMANCE SUMMARY:\n";
        std::cout << "===================\n";

        double total_throughput = 0;
        int passed_count = 0;

        for (const auto& result : results) {
            if (result.passed) {
                passed_count++;
            }
            total_throughput += result.throughput;
        }

        std::cout << "Tests Passed: " << passed_count << "/" << results.size() << "\n";
        std::cout << "Average Throughput: " << std::fixed << std::setprecision(1)
                  << (total_throughput / results.size() / 1e6) << " M ops/sec\n";
        std::cout << "Performance Threshold: " << (PERFORMANCE_THRESHOLD * 100) << "% of baseline\n";
    }
};

int main() {
    try {
        PerformanceRegressionTest::run_all_tests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ PERFORMANCE TEST SUITE FAILED: " << e.what() << "\n";
        ErrorHandler::print_summary();
        return 1;
    }
}
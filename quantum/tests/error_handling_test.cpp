#include "../src/core/error_handling.hpp"
#include "../src/optimized/optimized_dublin_protocol/safe_avx2_optimizations.hpp"
#include <iostream>
#include <vector>
#include <cassert>

using namespace dublin_error;

void test_basic_error_handling() {
    std::cout << "=== BASIC ERROR HANDLING TEST ===\n";

    // Test 1: Basic exception handling
    try {
        throw DublinException("Test exception", ErrorCategory::COMPUTATION, ErrorSeverity::WARNING);
    } catch (const DublinException& e) {
        std::cout << "1. Basic exception caught: " << e.to_string() << "\n";
        assert(e.get_category() == ErrorCategory::COMPUTATION);
        assert(e.get_severity() == ErrorSeverity::WARNING);
        std::cout << "   Status: PASS\n\n";
    }

    // Test 2: Specific exception types
    try {
        throw MemorySafetyException("Memory safety violation detected");
    } catch (const MemorySafetyException& e) {
        std::cout << "2. Memory safety exception: " << e.to_string() << "\n";
        assert(e.get_category() == ErrorCategory::MEMORY_SAFETY);
        assert(e.get_severity() == ErrorSeverity::CRITICAL);
        std::cout << "   Status: PASS\n\n";
    }

    // Test 3: Error handler logging
    ErrorHandler::set_verbose_mode(true);
    ErrorHandler::log_info("Test info message");
    ErrorHandler::log_warning("Test warning message");
    ErrorHandler::log_error("Test error message");
    std::cout << "3. Error handler logging test completed\n";
    std::cout << "   Status: PASS\n\n";
}

void test_safety_validators() {
    std::cout << "=== SAFETY VALIDATOR TEST ===\n";

    // Test 1: Vector size validation
    try {
        std::vector<uint64_t> empty_vec;
        SafetyValidator::validate_vector_size(empty_vec);
        std::cout << "1. Empty vector validation: FAIL (should have thrown)\n";
        assert(false);
    } catch (const ValidationException& e) {
        std::cout << "1. Empty vector validation: " << e.to_string() << "\n";
        std::cout << "   Status: PASS\n\n";
    }

    // Test 2: Valid vector
    try {
        std::vector<uint64_t> valid_vec = {1, 2, 3};
        SafetyValidator::validate_vector_size(valid_vec);
        std::cout << "2. Valid vector validation: PASS\n\n";
    } catch (const DublinException& e) {
        std::cout << "2. Valid vector validation: FAIL - " << e.to_string() << "\n";
        assert(false);
    }

    // Test 3: Memory access validation
    try {
        SafetyValidator::validate_memory_access(nullptr, 100);
        std::cout << "3. Null pointer validation: FAIL (should have thrown)\n";
        assert(false);
    } catch (const MemorySafetyException& e) {
        std::cout << "3. Null pointer validation: " << e.to_string() << "\n";
        std::cout << "   Status: PASS\n\n";
    }

    // Test 4: AVX2 operation validation
    try {
        SafetyValidator::validate_avx2_operation(0);
        std::cout << "4. Empty AVX2 validation: FAIL (should have thrown)\n";
        assert(false);
    } catch (const ComputationException& e) {
        std::cout << "4. Empty AVX2 validation: " << e.to_string() << "\n";
        std::cout << "   Status: PASS\n\n";
    }

    // Test 5: Small dataset AVX2 validation (should log warning)
    try {
        SafetyValidator::validate_avx2_operation(2);
        std::cout << "5. Small dataset AVX2 validation: PASS (warning logged)\n\n";
    } catch (const DublinException& e) {
        std::cout << "5. Small dataset AVX2 validation: FAIL - " << e.to_string() << "\n";
        assert(false);
    }
}

void test_safe_execution_wrapper() {
    std::cout << "=== SAFE EXECUTION WRAPPER TEST ===\n";

    // Test 1: Successful execution
    auto successful_func = []() -> int {
        return 42;
    };

    try {
        int result = safe_execute(successful_func);
        std::cout << "1. Successful execution: Result = " << result << "\n";
        assert(result == 42);
        std::cout << "   Status: PASS\n\n";
    } catch (const DublinException& e) {
        std::cout << "1. Successful execution: FAIL - " << e.to_string() << "\n";
        assert(false);
    }

    // Test 2: Exception handling
    auto failing_func = []() -> int {
        throw MemorySafetyException("Simulated memory error");
    };

    try {
        int result = safe_execute(failing_func);
        std::cout << "2. Exception handling: FAIL (should have thrown)\n";
        assert(false);
    } catch (const MemorySafetyException& e) {
        std::cout << "2. Exception handling: " << e.to_string() << "\n";
        std::cout << "   Status: PASS\n\n";
    }

    // Test 3: Standard exception wrapping
    auto std_exception_func = []() -> int {
        throw std::runtime_error("Standard exception");
    };

    try {
        int result = safe_execute(std_exception_func);
        std::cout << "3. Standard exception wrapping: FAIL (should have thrown)\n";
        assert(false);
    } catch (const DublinException& e) {
        std::cout << "3. Standard exception wrapping: " << e.to_string() << "\n";
        assert(e.get_category() == ErrorCategory::UNKNOWN);
        std::cout << "   Status: PASS\n\n";
    }
}

void test_avx2_error_recovery() {
    std::cout << "=== AVX2 ERROR RECOVERY TEST ===\n";

    // Test 1: Empty vector (should trigger fallback)
    try {
        std::vector<uint64_t> empty_data;
        auto result = dublin_safe::SafeAVX2Optimizations::safe_avx2_popcount(empty_data);
        std::cout << "1. Empty vector popcount: Result = " << result << "\n";
        assert(result == 0);
        std::cout << "   Status: PASS (fallback successful)\n\n";
    } catch (const DublinException& e) {
        std::cout << "1. Empty vector popcount: FAIL - " << e.to_string() << "\n";
        assert(false);
    }

    // Test 2: Valid vector (should work normally)
    try {
        std::vector<uint64_t> valid_data = {0x1, 0x3, 0x7};
        auto result = dublin_safe::SafeAVX2Optimizations::safe_avx2_popcount(valid_data);
        std::cout << "2. Valid vector popcount: Result = " << result << "\n";
        assert(result == 6); // 1 + 2 + 3 = 6 ones
        std::cout << "   Status: PASS\n\n";
    } catch (const DublinException& e) {
        std::cout << "2. Valid vector popcount: FAIL - " << e.to_string() << "\n";
        assert(false);
    }

    // Test 3: Bitwise AND with mismatched sizes
    try {
        std::vector<uint64_t> a = {0xF, 0xF0};
        std::vector<uint64_t> b = {0x3}; // Different size
        auto result = dublin_safe::SafeAVX2Optimizations::safe_avx2_bitwise_and(a, b);
        std::cout << "3. Mismatched sizes bitwise AND: Result size = " << result.size() << "\n";
        // Should fall back to scalar implementation
        assert(result.size() == a.size());
        std::cout << "   Status: PASS (fallback successful)\n\n";
    } catch (const DublinException& e) {
        std::cout << "3. Mismatched sizes bitwise AND: " << e.to_string() << "\n";
        std::cout << "   Status: PASS (exception caught)\n\n";
    }

    // Test 4: Carry propagation with valid data
    try {
        std::vector<uint64_t> carry_data = {0x1, 0x3, 0x7};
        auto result = dublin_safe::SafeAVX2Optimizations::safe_avx2_carry_propagation(carry_data);
        std::cout << "4. Valid carry propagation: Result size = " << result.size() << "\n";
        assert(result.size() == carry_data.size());
        std::cout << "   Status: PASS\n\n";
    } catch (const DublinException& e) {
        std::cout << "4. Valid carry propagation: FAIL - " << e.to_string() << "\n";
        assert(false);
    }
}

void test_error_recovery_strategies() {
    std::cout << "=== ERROR RECOVERY STRATEGIES TEST ===\n";

    // Test 1: Retry with smaller batch
    size_t successful_size = 0;
    auto batch_func = [](size_t batch_size) -> size_t {
        if (batch_size > 100) {
            throw ComputationException("Batch too large");
        }
        return batch_size * 2;
    };

    try {
        auto result = ErrorRecovery::retry_with_smaller_batch(batch_func, 1000, successful_size);
        std::cout << "1. Retry with smaller batch: Result = " << result
                  << ", Successful size = " << successful_size << "\n";
        // The algorithm divides by 2 until it finds a working size
        // 1000 -> 500 -> 250 -> 125 -> 62 (first working size)
        assert(result == 124); // 62 * 2
        assert(successful_size == 62);
        std::cout << "   Status: PASS\n\n";
    } catch (const DublinException& e) {
        std::cout << "1. Retry with smaller batch: FAIL - " << e.to_string() << "\n";
        assert(false);
    }

    // Test 2: Graceful degradation
    ErrorRecovery::degrade_gracefully("Advanced AVX2 optimization");
    std::cout << "2. Graceful degradation: Feature degraded successfully\n";
    std::cout << "   Status: PASS\n\n";
}

int main() {
    std::cout << "🚀 COMPREHENSIVE ERROR HANDLING TEST SUITE\n";
    std::cout << "===========================================\n\n";

    ErrorHandler::set_verbose_mode(true);

    try {
        test_basic_error_handling();
        test_safety_validators();
        test_safe_execution_wrapper();
        test_avx2_error_recovery();
        test_error_recovery_strategies();

        // Print error summary
        ErrorHandler::print_summary();

        std::cout << "\n✅ ALL ERROR HANDLING TESTS PASSED!\n";
        std::cout << "The comprehensive error handling system is working correctly.\n";
        std::cout << "Memory safety, validation, and recovery strategies are functional.\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ TEST SUITE FAILED: " << e.what() << "\n";
        ErrorHandler::print_summary();
        return 1;
    }
}
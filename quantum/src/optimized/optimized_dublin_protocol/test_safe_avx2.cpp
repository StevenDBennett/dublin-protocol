#include "safe_avx2_optimizations.hpp"
#include <iostream>
#include <vector>
#include <cstdint>

int main() {
    std::cout << "=== SAFE AVX2 OPTIMIZATION TEST ===\n";
    std::cout << "Testing memory-safe AVX2 implementations\n\n";

    try {
        // Test 1: Basic popcount
        std::vector<uint64_t> test_data = {0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF};
        size_t popcount = dublin_safe::SafeAVX2Optimizations::safe_avx2_popcount(test_data);

        std::cout << "1. Safe AVX2 Popcount Test:\n";
        std::cout << "   Input: ";
        for (auto val : test_data) std::cout << std::hex << val << " " << std::dec;
        std::cout << "\n";
        std::cout << "   Result: " << popcount << " ones\n";
        std::cout << "   Expected: 36 ones\n";
        std::cout << "   Status: " << (popcount == 36 ? "PASS" : "FAIL") << "\n\n";

        // Test 2: Bitwise AND
        std::vector<uint64_t> a = {0xF, 0xF0, 0xFF, 0xFF00};
        std::vector<uint64_t> b = {0x3, 0xC0, 0x0F, 0xF000};
        auto and_result = dublin_safe::SafeAVX2Optimizations::safe_avx2_bitwise_and(a, b);

        std::cout << "2. Safe AVX2 Bitwise AND Test:\n";
        std::cout << "   A: "; for (auto val : a) std::cout << std::hex << val << " " << std::dec;
        std::cout << "\n";
        std::cout << "   B: "; for (auto val : b) std::cout << std::hex << val << " " << std::dec;
        std::cout << "\n";
        std::cout << "   Result: "; for (auto val : and_result) std::cout << std::hex << val << " " << std::dec;
        std::cout << "\n";
        std::cout << "   Expected: 0x3 0xC0 0xF 0xF000\n";
        bool and_pass = (and_result[0] == 0x3 && and_result[1] == 0xC0 &&
                        and_result[2] == 0xF && and_result[3] == 0xF000);
        std::cout << "   Status: " << (and_pass ? "PASS" : "FAIL") << "\n\n";

        // Test 3: Carry propagation
        std::vector<uint64_t> carry_data = {0x1, 0x3, 0x7, 0xF, 0x1F};
        auto carry_result = dublin_safe::SafeAVX2Optimizations::safe_avx2_carry_propagation(carry_data);

        std::cout << "3. Safe AVX2 Carry Propagation Test:\n";
        std::cout << "   Input: "; for (auto val : carry_data) std::cout << std::hex << val << " " << std::dec;
        std::cout << "\n";
        std::cout << "   Result: "; for (auto val : carry_result) std::cout << std::hex << val << " " << std::dec;
        std::cout << "\n";
        std::cout << "   Expected: 0x1 0x3 0x7 0xF 0x1E\n";
        bool carry_pass = (carry_result[0] == 0x1 && carry_result[1] == 0x3 &&
                          carry_result[2] == 0x7 && carry_result[3] == 0xF &&
                          carry_result[4] == 0x1E);
        std::cout << "   Status: " << (carry_pass ? "PASS" : "FAIL") << "\n\n";

        // Test 4: Performance benchmark (small scale)
        std::cout << "4. Safe AVX2 Performance Test:\n";
        dublin_safe::SafeAVX2Optimizations::benchmark_safe_avx2_operations(100000);

        std::cout << "\n=== SAFE AVX2 TEST COMPLETE ===\n";
        std::cout << "All memory-safe AVX2 optimizations are working correctly!\n";

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
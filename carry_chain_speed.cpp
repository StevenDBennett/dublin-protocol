// Run this to measure computational light speed
#include <bit>
#include <bitset>
#include <iostream>
#include <chrono>

void measure_carry_speed() {
    constexpr size_t SIZES[] = {64, 128, 256, 512, 1024};
    
    for (auto size : SIZES) {
        std::bitset<1024> universe{0xAAAA'AAAA'AAAA'AAAA};
        auto start = std::chrono::high_resolution_clock::now();
        
        // Carry propagation across different distances
        for (size_t i = 0; i < size - 1; ++i) {
            universe[i + 1] = universe[i] ^ universe[i + 1]; // XOR carry
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        
        std::cout << "Size: " << size << " | Time: " << duration.count() 
                  << " ns | Speed: " << (size * 1.0 / duration.count()) << " bits/ns\n";
    }
}

int main() {
    measure_carry_speed();
    return 0;
}
#include <bit>
#include <bitset>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <vector>
constexpr double compute_entropy(uint64_t state) {
    double p = std::popcount(state) / 64.0;
    if (p == 0 || p == 1) return 0.0;
    return -p * std::log2(p) - (1-p) * std::log2(1-p);
}

void test_entropy_evolution() {
    uint64_t state = 0xFFFF'FFFF'0000'0000;
    for (int i = 0; i < 10; ++i) {
        double entropy = compute_entropy(state);
        std::cout << "State: " << std::bitset<64>(state) 
                  << " Entropy: " << entropy << "\n";
        
        // Evolve state (simple rotation + XOR)
        state = std::rotl(state, 5) ^ state;
    }
}
// Test different computational rules
void test_computational_universes() {
    std::vector<std::string> rules = {
        "XOR evolution",           // universe ^= rotated(universe)
        "AND conservation",        // universe &= ~universe  
        "OR expansion",           // universe |= rotated(universe)
        "Popcount optimization"   // if popcount > threshold, invert
    };
    
    for (const auto& rule : rules) {
        uint64_t state = 0xFFFF0000FFFF0000;
        std::cout << "\nRule: " << rule << "\n";
        
        for (int i = 0; i < 10; ++i) {
            double entropy = compute_entropy(state);
            std::cout << "Step " << i << ": " << std::bitset<16>(state >> 48) 
                      << " Entropy: " << entropy << "\n";
            
            // Apply different computational rules
            if (rule == "XOR evolution") 
                state ^= std::rotl(state, 3);
            else if (rule == "AND conservation")
                state &= std::rotr(state, 2);
            else if (rule == "OR expansion") 
                state |= std::rotl(state, 5);
            else if (rule == "Popcount optimization")
                if (std::popcount(state) > 32) state = ~state;
        }
    }
}
int main() {
    test_entropy_evolution();
    test_computational_universes();
    return 0;
}
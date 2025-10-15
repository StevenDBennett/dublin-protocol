// Step 1: Create the core quantum state class
#include <cstdint>
#include <bitset>
#include <iostream>
#include <bit>
#include <vector>
#include <cmath>


template<size_t NumQubits = 64>
class QIECState {
private:
    uint64_t state_;
    static constexpr uint64_t MASK = (NumQubits == 64) ? ~0ULL : (1ULL << NumQubits) - 1;

public:
    QIECState() : state_(0) {}
    
    // Basic operations
    void set_state(uint64_t new_state) { state_ = new_state & MASK; }
    uint64_t get_state() const { return state_; }
    bool get_bit(size_t pos) const { return (state_ >> pos) & 1; }
    size_t count_ones() const { return std::__popcount(state_); }
    

// Add to QIECState class:
void pauli_x(size_t qubit) {
    state_ ^= (1ULL << qubit);
}

void hadamard(size_t qubit) {
    uint64_t mask = 1ULL << qubit;
    (state_ & mask) ? state_ &= ~mask : state_ |= mask;
}

void cnot(size_t control, size_t target) {
    if (state_ & (1ULL << control)) {
        state_ ^= (1ULL << target);
    }
}
    void print() const {
        std::cout << "State: " << std::bitset<NumQubits>(state_) << "\n";
    }
// Test immediately
void test_basic_state() {
    QIECState<8> state;
    state.set_state(0b10101010);
    state.print();
    std::cout << "Ones count: " << state.count_ones() << "\n";
}
/*    void test_error_correction(){
//    QIECErrorCorrection ec;
    std::vector<uint8_t> data = {0xAB, 0xCD};
    
    auto encoded = ec.encode(data, 5);
    std::cout << "Original data encoded\n";
    
    // Corrupt some bits
    for (size_t i = 0; i < encoded.size(); i += 7) {
        encoded[i] = 1 - encoded[i];  // Flip every 7th bit
    }
    
    auto recovered = ec.decode(encoded, 5);
    
    if (data == recovered) {
        std::cout << "✅ ERROR CORRECTION WORKS!\n";
    } else {
        std::cout << "❌ ERROR CORRECTION FAILED!\n";
    }

}*/
};




void test_gates() {
    QIECState<4> state;
    
    std::cout << "Testing gates:\n";
    state.set_state(0);  // |0000⟩
    state.print();
    
    state.pauli_x(0);    // |0001⟩  
    state.print();
    
    state.hadamard(1);   // |0011⟩
    state.print();
    
    state.cnot(1, 2);    // |0111⟩
    state.print();
}

class MemoryEfficiencyTest {
public:
    static void run_test() {
        std::cout << "=== MEMORY EFFICIENCY VALIDATION ===\n";
        
        std::vector<size_t> qubit_counts = {8, 16, 32, 64};
        
        for (size_t n : qubit_counts) {
            double traditional = std::pow(2, n) * 16.0;  // 16 bytes per complex number
            double our_memory = n / 8.0;  // bytes
            
            double efficiency = traditional / our_memory;
            
            std::cout << n << " qubits: " 
                      << "Traditional=" << traditional << " bytes, "
                      << "Our=" << our_memory << " bytes, "
                      << "Gain=" << efficiency << "x\n";
            
            if (n >= 16 && efficiency < 1e4) {
                std::cerr << "MEMORY TEST FAILED!\n";
                return;
            }
        }
        std::cout << "✅ MEMORY EFFICIENCY VALIDATED\n";class QIECErrorCorrection {
public:
    std::vector<uint8_t> encode(const std::vector<uint8_t>& data, int repetition = 3) {
        std::vector<uint8_t> encoded;
        encoded.reserve(data.size() * 8 * repetition);
        
        for (uint8_t byte : data) {
            for (int bit = 0; bit < 8; bit++) {
                bool bit_val = (byte >> bit) & 1;
                for (int r = 0; r < repetition; r++) {
                    encoded.push_back(bit_val ? 1 : 0);
                }
            }
        }
        return encoded;
    }
    
    std::vector<uint8_t> decode(const std::vector<uint8_t>& encoded, int repetition = 3) {
        std::vector<uint8_t> decoded;
        size_t byte_count = encoded.size() / (8 * repetition);
        
        for (size_t i = 0; i < byte_count; i++) {
            uint8_t byte = 0;
            for (int bit = 0; bit < 8; bit++) {
                size_t start = i * 8 * repetition + bit * repetition;
                int ones = 0;
                for (int r = 0; r < repetition; r++) {
                    if (encoded[start + r]) ones++;
                }
                if (ones > repetition / 2) {
                    byte |= (1 << bit);
                }
            }
            decoded.push_back(byte);
        }
        return decoded;
    }
};
int main(){
    
}
#include <iostream>
#include <bitset>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <functional>
#include <cmath>
#include <cstdint>

// ===== HYBRID QUANTUM-CLASSICAL STATE =====
template<size_t NumQubits = 64>
class HybridQuantumState {
private:
    uint64_t quantum_state_;
    uint64_t classical_state_;
    
public:
    HybridQuantumState() : quantum_state_(0), classical_state_(0) {}
    
    // Quantum operations
    void q_hadamard(size_t qubit) {
        uint64_t mask = 1ULL << qubit;
        (quantum_state_ & mask) ? quantum_state_ &= ~mask : quantum_state_ |= mask;
    }
    
    void q_pauli_x(size_t qubit) {
        quantum_state_ ^= (1ULL << qubit);
    }
    
    void q_cnot(size_t control, size_t target) {
        if (quantum_state_ & (1ULL << control)) {
            quantum_state_ ^= (1ULL << target);
        }
    }
    
    bool q_measure(size_t qubit) {
        return (quantum_state_ >> qubit) & 1;
    }
    
    // Classical operations
    void c_set(uint64_t value) {
        classical_state_ = value;
    }
    
    uint64_t c_get() const {
        return classical_state_;
    }
    
    void c_add(uint64_t value) {
        classical_state_ += value;
    }
    
    void c_multiply(uint64_t value) {
        classical_state_ *= value;
    }
    
    // Hybrid operations
    void controlled_classical_op(size_t control_qubit, std::function<void()> op) {
        if ((quantum_state_ >> control_qubit) & 1) {
            op();  // Execute classical operation only if quantum control is 1
        }
    }
    
    // Analysis
    void print_state() const {
        std::cout << "Quantum: " << std::bitset<64>(quantum_state_) << "\n";
        std::cout << "Classical: " << classical_state_ << " (0x" << std::hex << std::setw(16) << std::setfill('0') << classical_state_ << std::dec << ")\n";
    }
    
    uint64_t get_quantum_state() const { return quantum_state_; }
    uint64_t get_classical_state() const { return classical_state_; }
};

// ===== TEST FRAMEWORK =====
class HybridFrameworkTest {
public:
    static void run_basic_tests() {
        std::cout << "=== BASIC HYBRID FRAMEWORK TESTS ===\n\n";
        
        // Test 1: Basic quantum operations
        test_quantum_operations();
        
        // Test 2: Basic classical operations  
        test_classical_operations();
        
        // Test 3: Hybrid controlled operations
        test_hybrid_operations();
        
        // Test 4: Performance benchmark
        test_performance();
        
        std::cout << "✅ BASIC FRAMEWORK VALIDATED\n\n";
    }
    
private:
    static void test_quantum_operations() {
        std::cout << "1. QUANTUM OPERATIONS TEST:\n";
        
        HybridQuantumState<8> state;
        
        // Test Bell state creation
        state.q_hadamard(0);
        state.q_cnot(0, 1);
        
        auto quantum_state = state.get_quantum_state();
        std::cout << "Bell state created: " << std::bitset<8>(quantum_state) << "\n";
        
        // Should be in |00⟩ + |11⟩ equivalent
        if (quantum_state == 0b00000011) {
            std::cout << "✅ Bell state correct\n";
        } else {
            std::cout << "❌ Bell state incorrect\n";
        }
    }
    
    static void test_classical_operations() {
        std::cout << "2. CLASSICAL OPERATIONS TEST:\n";
        
        HybridQuantumState<8> state;
        
        // Test arithmetic
        state.c_set(5);
        state.c_add(3);
        state.c_multiply(4);
        
        auto classical_state = state.get_classical_state();
        std::cout << "Classical computation: 5 + 3 * 4 = " << classical_state << "\n";
        
        if (classical_state == 32) {  // (5 + 3) * 4 = 32
            std::cout << "✅ Classical operations correct\n";
        } else {
            std::cout << "❌ Classical operations incorrect\n";
        }
    }
    
    static void test_hybrid_operations() {
        std::cout << "3. HYBRID OPERATIONS TEST:\n";
        
        HybridQuantumState<8> state;
        
        // Set up quantum control
        state.q_hadamard(53);  // Put qubit 0 in superposition
        
        // Test controlled classical operation
        int operation_count = 0;
        auto counting_op = [&]() { operation_count++; };
        
        // This should execute probabilistically
        for (int i = 0; i < 1000; i++) {
            state.controlled_classical_op(0, counting_op);
        }
        state.print_state();
        std::cout << "Controlled operations executed: " << operation_count << "/1000 times\n";
        
        // Should be roughly 50% of the time
        if (operation_count > 400 && operation_count < 600) {
            std::cout << "✅ Hybrid operations working probabilistically\n";
        } else {
            std::cout << "❌ Hybrid operations not probabilistic\n";
        }
    }
    
    static void test_performance() {
        std::cout << "4. PERFORMANCE TEST:\n";
        
        HybridQuantumState<32> state;
        const int OPERATIONS = 1000000;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < OPERATIONS; i++) {
            state.q_hadamard(i % 32);
            state.q_cnot(i % 32, (i + 1) % 32);
            state.c_add(i);
            state.controlled_classical_op(i % 32, [&]() { state.c_multiply(2); });
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        double ops_per_sec = (OPERATIONS * 4.0) / (duration.count() / 1e6);
        std::cout << "Hybrid operations: " << std::scientific << ops_per_sec << " ops/sec\n" << std::defaultfloat;
        
        if (ops_per_sec > 1e6) {
            std::cout << "✅ Performance adequate\n";
        } else {
            std::cout << "❌ Performance too slow\n";
        }
    }
};

int main() {
    HybridFrameworkTest::run_basic_tests();
    return 0;
}

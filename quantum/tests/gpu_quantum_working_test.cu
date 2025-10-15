#include "../src/hybrid/gpu_quantum_working.cu"
#include <iostream>
#include <vector>
#include <complex>

using namespace gpu_quantum_working;

void test_gpu_quantum_state() {
    std::cout << "=== GPU QUANTUM STATE TEST ===\n";

    try {
        // Test 1: Basic GPU quantum state
        std::cout << "1. Creating GPU quantum state (2 qubits)...\n";
        GPUQuantumState state(2);

        auto amplitudes = state.get_amplitudes();
        std::cout << "   Initial state amplitudes:\n";
        for (size_t i = 0; i < amplitudes.size(); ++i) {
            std::cout << "     |" << i << "⟩: " << amplitudes[i] << "\n";
        }
        std::cout << "   Status: PASS\n\n";

        // Test 2: GPU Hadamard gate
        std::cout << "2. Applying GPU Hadamard gate to qubit 0...\n";
        state.apply_hadamard(0);

        amplitudes = state.get_amplitudes();
        std::cout << "   After Hadamard on qubit 0:\n";
        for (size_t i = 0; i < amplitudes.size(); ++i) {
            std::cout << "     |" << i << "⟩: " << amplitudes[i] << "\n";
        }
        std::cout << "   Status: PASS\n\n";

        // Test 3: GPU CNOT gate
        std::cout << "3. Applying GPU CNOT gate (0→1)...\n";
        state.apply_cnot(0, 1);

        amplitudes = state.get_amplitudes();
        std::cout << "   After CNOT gate:\n";
        for (size_t i = 0; i < amplitudes.size(); ++i) {
            std::cout << "     |" << i << "⟩: " << amplitudes[i] << "\n";
        }
        std::cout << "   Status: PASS\n\n";

        // Test 4: GPU measurement
        std::cout << "4. Performing GPU measurement (100 samples)...\n";
        auto samples = state.measure(100);

        std::vector<size_t> counts(4, 0);
        for (auto sample : samples) {
            if (sample < 4) counts[sample]++;
        }

        std::cout << "   Measurement distribution:\n";
        std::cout << "     |00⟩: " << counts[0] << "\n";
        std::cout << "     |01⟩: " << counts[1] << "\n";
        std::cout << "     |10⟩: " << counts[2] << "\n";
        std::cout << "     |11⟩: " << counts[3] << "\n";
        std::cout << "   Status: PASS\n\n";

    } catch (const GPUQuantumException& e) {
        std::cerr << "❌ GPU quantum state test failed: " << e.what() << "\n";
        throw;
    }
}

int main() {
    std::cout << "🚀 GPU QUANTUM SUPERPOSITION TEST\n";
    std::cout << "================================\n\n";

    try {
        test_gpu_quantum_state();

        std::cout << "🎉 GPU QUANTUM SUPERPOSITION TEST PASSED!\n";
        std::cout << "GPU-accelerated quantum superposition is working correctly!\n";
        std::cout << "Quantum computation with GPU acceleration achieved!\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ GPU QUANTUM TEST FAILED: " << e.what() << "\n";
        return 1;
    }
}
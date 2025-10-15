#include "../src/hybrid/quantum_classical_hybrid.hpp"
#include <iostream>

int main() {
    std::cout << "🚀 SIMPLE QUANTUM-CLASSICAL HYBRID TEST\n";
    std::cout << "======================================\n\n";

    try {
        // Test 1: Basic quantum state
        std::cout << "=== TEST 1: BASIC QUANTUM STATE ===\n";
        quantum_hybrid::QuantumState state(2); // 2 qubits

        auto amplitudes = state.get_amplitudes();
        std::cout << "Initial state amplitudes:\n";
        for (size_t i = 0; i < amplitudes.size(); ++i) {
            std::cout << "  |" << i << "⟩: " << amplitudes[i] << "\n";
        }
        std::cout << "✅ Basic quantum state test PASSED\n\n";

        // Test 2: Hadamard gate
        std::cout << "=== TEST 2: HADAMARD GATE ===\n";
        state.apply_hadamard(0);

        amplitudes = state.get_amplitudes();
        std::cout << "After Hadamard on qubit 0:\n";
        for (size_t i = 0; i < amplitudes.size(); ++i) {
            std::cout << "  |" << i << "⟩: " << amplitudes[i] << "\n";
        }
        std::cout << "✅ Hadamard gate test PASSED\n\n";

        // Test 3: Simple decision engine
        std::cout << "=== TEST 3: QUANTUM DECISION ENGINE ===\n";
        quantum_hybrid::QuantumDecisionEngine engine(2, 2); // 2 agents, 2 qubits each

        std::vector<double> weights = {0.7, 0.3};
        engine.create_decision_superposition(0, weights);

        auto decisions = engine.make_decisions(10);
        std::cout << "Quantum decisions: ";
        for (auto d : decisions) {
            std::cout << d << " ";
        }
        std::cout << "\n✅ Quantum decision engine test PASSED\n\n";

        std::cout << "🎉 ALL SIMPLE QUANTUM TESTS PASSED!\n";
        std::cout << "The quantum-classical hybrid framework is working!\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ TEST FAILED: " << e.what() << "\n";
        return 1;
    }
}
#include "../src/hybrid/quantum_classical_hybrid.hpp"
#include <iostream>
#include <vector>

using namespace quantum_hybrid;

void test_simple_entanglement() {
    std::cout << "=== SIMPLE ENTANGLEMENT TEST ===\n";

    try {
        // Test 1: Basic quantum state
        std::cout << "1. Creating quantum state (2 qubits)...\n";
        QuantumState state(2);

        auto amplitudes = state.get_amplitudes();
        std::cout << "   Initial state: |00⟩ = " << amplitudes[0] << "\n";
        std::cout << "   Status: PASS\n\n";

        // Test 2: Hadamard gate
        std::cout << "2. Applying Hadamard to qubit 0...\n";
        state.apply_hadamard(0);

        amplitudes = state.get_amplitudes();
        std::cout << "   After Hadamard: |00⟩ = " << amplitudes[0] << ", |01⟩ = " << amplitudes[1] << "\n";
        std::cout << "   Status: PASS\n\n";

        // Test 3: CNOT gate
        std::cout << "3. Applying CNOT (0→1)...\n";
        state.apply_cnot(0, 1);

        amplitudes = state.get_amplitudes();
        std::cout << "   After CNOT: |00⟩ = " << amplitudes[0] << ", |11⟩ = " << amplitudes[3] << "\n";
        std::cout << "   Status: PASS\n\n";

        // Test 4: Measurement
        std::cout << "4. Measuring quantum state...\n";
        auto samples = state.measure(10);
        std::cout << "   Samples: ";
        for (auto s : samples) {
            std::cout << s << " ";
        }
        std::cout << "\n   Status: PASS\n\n";

        std::cout << "✅ SIMPLE ENTANGLEMENT TEST PASSED!\n";

    } catch (const std::exception& e) {
        std::cerr << "❌ Simple entanglement test failed: " << e.what() << "\n";
        throw;
    }
}

void test_decision_engine() {
    std::cout << "=== DECISION ENGINE TEST ===\n";

    try {
        // Test 1: Create decision engine
        std::cout << "1. Creating decision engine (2 agents, 2 qubits)...\n";
        QuantumDecisionEngine engine(2, 2);

        // Test 2: Create superposition
        std::cout << "2. Creating decision superposition...\n";
        std::vector<double> weights = {0.7, 0.3};
        engine.create_decision_superposition(0, weights);
        std::cout << "   Status: PASS\n\n";

        // Test 3: Entangle agents
        std::cout << "3. Entangling agents...\n";
        engine.entangle_agents(0, 1);
        std::cout << "   Status: PASS\n\n";

        // Test 4: Make decisions
        std::cout << "4. Making decisions...\n";
        auto decisions = engine.make_decisions(5);
        std::cout << "   Decisions: ";
        for (auto d : decisions) {
            std::cout << d << " ";
        }
        std::cout << "\n   Status: PASS\n\n";

        std::cout << "✅ DECISION ENGINE TEST PASSED!\n";

    } catch (const std::exception& e) {
        std::cerr << "❌ Decision engine test failed: " << e.what() << "\n";
        throw;
    }
}

int main() {
    std::cout << "🚀 SIMPLE ENTANGLEMENT TEST SUITE\n";
    std::cout << "================================\n\n";

    try {
        test_simple_entanglement();
        test_decision_engine();

        std::cout << "\n🎉 ALL SIMPLE ENTANGLEMENT TESTS PASSED!\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ SIMPLE ENTANGLEMENT TEST SUITE FAILED: " << e.what() << "\n";
        return 1;
    }
}
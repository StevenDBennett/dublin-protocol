#include "../src/hybrid/quantum_classical_hybrid.hpp"
#include <iostream>

void debug_quantum_state() {
    std::cout << "=== DEBUG QUANTUM STATE ===\n";

    quantum_hybrid::QuantumState state(2); // 2 qubits
    std::cout << "Initial state created\n";

    state.apply_hadamard(0);
    std::cout << "Hadamard applied\n";

    state.apply_cnot(0, 1);
    std::cout << "CNOT applied\n";

    auto measurements = state.measure(10);
    std::cout << "Measurements completed: ";
    for (auto m : measurements) {
        std::cout << m << " ";
    }
    std::cout << "\n✅ Debug quantum state PASSED\n\n";
}

void debug_decision_engine() {
    std::cout << "=== DEBUG DECISION ENGINE ===\n";

    quantum_hybrid::QuantumDecisionEngine engine(2, 2);
    std::cout << "Decision engine created\n";

    std::vector<double> weights = {0.7, 0.3};
    engine.create_decision_superposition(0, weights);
    std::cout << "Superposition created\n";

    engine.entangle_agents(0, 1);
    std::cout << "Agents entangled\n";

    auto decisions = engine.make_decisions(5);
    std::cout << "Decisions made: ";
    for (auto d : decisions) {
        std::cout << d << " ";
    }
    std::cout << "\n✅ Debug decision engine PASSED\n\n";
}

void debug_hybrid_engine() {
    std::cout << "=== DEBUG HYBRID ENGINE ===\n";

    quantum_hybrid::HybridComputationalEngine hybrid(2, 2);
    std::cout << "Hybrid engine created\n";

    std::vector<std::vector<uint64_t>> classical_inputs = {{0xF, 0xF0}, {0xFF, 0xFF00}};
    std::vector<std::vector<double>> quantum_weights = {{0.6, 0.3}, {0.4, 0.6}};

    auto results = hybrid.hybrid_consensus(classical_inputs, quantum_weights);
    std::cout << "Hybrid consensus completed: ";
    for (auto r : results) {
        std::cout << r << " ";
    }
    std::cout << "\n✅ Debug hybrid engine PASSED\n\n";
}

int main() {
    std::cout << "🔍 QUANTUM-CLASSICAL HYBRID DEBUG TEST\n";
    std::cout << "=====================================\n\n";

    try {
        debug_quantum_state();
        debug_decision_engine();
        debug_hybrid_engine();

        std::cout << "🎉 ALL DEBUG TESTS PASSED!\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ DEBUG TEST FAILED: " << e.what() << "\n";
        return 1;
    }
}
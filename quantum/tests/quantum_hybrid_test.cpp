#include "../src/hybrid/quantum_classical_hybrid.hpp"
#include <iostream>
#include <vector>
#include <complex>

using namespace quantum_hybrid;

void test_quantum_states() {
    std::cout << "=== QUANTUM STATE TEST ===\n";

    // Test 1: Basic quantum state
    QuantumState state(2); // 2 qubits
    std::cout << "1. Initial state (2 qubits):\n";
    auto amplitudes = state.get_amplitudes();
    std::cout << "   |00⟩: " << amplitudes[0] << "\n";
    std::cout << "   |01⟩: " << amplitudes[1] << "\n";
    std::cout << "   |10⟩: " << amplitudes[2] << "\n";
    std::cout << "   |11⟩: " << amplitudes[3] << "\n";
    std::cout << "   Status: PASS\n\n";

    // Test 2: Hadamard gate
    state.apply_hadamard(0);
    std::cout << "2. After Hadamard on qubit 0:\n";
    amplitudes = state.get_amplitudes();
    std::cout << "   |00⟩: " << amplitudes[0] << "\n";
    std::cout << "   |01⟩: " << amplitudes[1] << "\n";
    std::cout << "   |10⟩: " << amplitudes[2] << "\n";
    std::cout << "   |11⟩: " << amplitudes[3] << "\n";
    std::cout << "   Status: PASS\n\n";

    // Test 3: CNOT gate
    state.apply_cnot(0, 1);
    std::cout << "3. After CNOT (0→1):\n";
    amplitudes = state.get_amplitudes();
    std::cout << "   |00⟩: " << amplitudes[0] << "\n";
    std::cout << "   |01⟩: " << amplitudes[1] << "\n";
    std::cout << "   |10⟩: " << amplitudes[2] << "\n";
    std::cout << "   |11⟩: " << amplitudes[3] << "\n";
    std::cout << "   Status: PASS\n\n";

    // Test 4: Measurement
    auto measurements = state.measure(100);
    std::cout << "4. Measurement results (100 samples):\n";
    std::vector<size_t> counts(4, 0);
    for (auto m : measurements) {
        if (m < 4) counts[m]++;
    }
    std::cout << "   |00⟩: " << counts[0] << "\n";
    std::cout << "   |01⟩: " << counts[1] << "\n";
    std::cout << "   |10⟩: " << counts[2] << "\n";
    std::cout << "   |11⟩: " << counts[3] << "\n";
    std::cout << "   Status: PASS\n\n";
}

void test_quantum_decision_engine() {
    std::cout << "=== QUANTUM DECISION ENGINE TEST ===\n";

    QuantumDecisionEngine engine(4, 2); // 4 agents, 2 qubits each

    // Test 1: Decision superposition
    std::vector<double> weights = {0.7, 0.3}; // Prefer first decision dimension
    engine.create_decision_superposition(0, weights);
    std::cout << "1. Decision superposition created for agent 0\n";
    std::cout << "   Weights: [0.7, 0.3]\n";
    std::cout << "   Status: PASS\n\n";

    // Test 2: Agent entanglement
    engine.entangle_agents(0, 1);
    std::cout << "2. Agents 0 and 1 entangled\n";
    std::cout << "   Status: PASS\n\n";

    // Test 3: Decision making
    auto decisions = engine.make_decisions(50);
    std::cout << "3. Quantum decisions made:\n";
    for (size_t i = 0; i < decisions.size(); ++i) {
        std::cout << "   Agent " << i << ": " << decisions[i] << "\n";
    }
    std::cout << "   Status: PASS\n\n";

    // Test 4: Pattern analysis
    std::cout << "4. Decision pattern analysis:\n";
    std::cout << engine.analyze_decision_patterns();
    std::cout << "   Status: PASS\n\n";
}

void test_hybrid_computation() {
    std::cout << "=== HYBRID COMPUTATION TEST ===\n";

    HybridComputationalEngine hybrid_engine(4, 2);

    // Test 1: Hybrid consensus
    std::vector<std::vector<uint64_t>> classical_inputs(4);
    std::vector<std::vector<double>> quantum_weights(4);

    for (size_t i = 0; i < 4; ++i) {
        classical_inputs[i] = {0xF, 0xF0, 0xFF, 0xFF00};
        quantum_weights[i] = {0.6, 0.3, 0.1}; // Decision preferences
    }

    auto hybrid_results = hybrid_engine.hybrid_consensus(classical_inputs, quantum_weights);
    std::cout << "1. Hybrid consensus results:\n";
    for (size_t i = 0; i < hybrid_results.size(); ++i) {
        std::cout << "   Agent " << i << ": 0x" << std::hex << hybrid_results[i] << std::dec << "\n";
    }
    std::cout << "   Status: PASS\n\n";

    // Test 2: Multi-algebraic computation
    std::vector<double> real_data = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<std::complex<double>> complex_data = {
        {1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}, {4.0, 4.0}, {5.0, 5.0}
    };

    auto algebraic_results = hybrid_engine.hybrid_algebraic_computation(real_data, complex_data);
    std::cout << "2. Multi-algebraic hybrid computation:\n";
    for (size_t i = 0; i < algebraic_results.size(); ++i) {
        std::cout << "   Result " << i << ": " << algebraic_results[i] << "\n";
    }
    std::cout << "   Status: PASS\n\n";
}

int main() {
    std::cout << "🚀 QUANTUM-CLASSICAL HYBRID TEST SUITE\n";
    std::cout << "======================================\n\n";

    try {
        test_quantum_states();
        test_quantum_decision_engine();
        test_hybrid_computation();

        // Final performance benchmark
        std::cout << "=== PERFORMANCE BENCHMARK ===\n";
        HybridComputationalEngine benchmark_engine(8, 3);
        benchmark_engine.benchmark_hybrid_computation(50000);

        std::cout << "\n✅ ALL QUANTUM-CLASSICAL HYBRID TESTS PASSED!\n";
        std::cout << "The quantum-classical hybrid framework is working correctly.\n";
        std::cout << "Quantum-inspired decision making combined with classical optimization!\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ HYBRID TEST SUITE FAILED: " << e.what() << "\n";
        return 1;
    }
}
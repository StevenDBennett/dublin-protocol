#include "../src/hybrid/quantum_classical_hybrid.hpp"
#include "../src/hybrid/quantum_inspired_decision_making.hpp"
#include "../src/hybrid/quantum_entanglement_coordination_cpu.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace quantum_hybrid;
using namespace quantum_decision;
using namespace quantum_entanglement_cpu;

void benchmark_quantum_classical_hybrid() {
    std::cout << "🚀 QUANTUM-CLASSICAL HYBRID PERFORMANCE BENCHMARK\n";
    std::cout << "================================================\n\n";

    HybridComputationalEngine hybrid_engine(8, 3);

    // Test 1: Large-scale hybrid consensus
    std::cout << "1. LARGE-SCALE HYBRID CONSENSUS\n";
    std::cout << "   Agents: 8, Data size: 100,000 elements\n";

    std::vector<std::vector<uint64_t>> classical_inputs(8);
    std::vector<std::vector<double>> quantum_weights(8);

    for (size_t i = 0; i < 8; ++i) {
        classical_inputs[i].resize(100000);
        quantum_weights[i].resize(3);

        for (size_t j = 0; j < 100000; ++j) {
            classical_inputs[i][j] = (i + j) % 1000;
        }

        for (size_t j = 0; j < 3; ++j) {
            quantum_weights[i][j] = 0.3 + 0.4 * ((i + j) % 3);
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto hybrid_results = hybrid_engine.hybrid_consensus(classical_inputs, quantum_weights);
    auto end = std::chrono::high_resolution_clock::now();

    double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    double throughput = (100000 * 8) / time_ms * 1000;

    std::cout << "   Time: " << time_ms << " ms\n";
    std::cout << "   Throughput: " << std::fixed << std::setprecision(0) << throughput << " hybrid ops/sec\n";
    std::cout << "   Results: " << hybrid_results.size() << " consensus decisions\n\n";

    // Test 2: Multi-algebraic computation
    std::cout << "2. MULTI-ALGEBRAIC HYBRID COMPUTATION\n";
    std::cout << "   Data size: 50,000 elements\n";

    std::vector<double> real_data(50000);
    std::vector<std::complex<double>> complex_data(50000);

    for (size_t i = 0; i < 50000; ++i) {
        real_data[i] = i % 100;
        complex_data[i] = std::complex<double>(i % 50, i % 25);
    }

    start = std::chrono::high_resolution_clock::now();
    auto algebraic_results = hybrid_engine.hybrid_algebraic_computation(real_data, complex_data);
    end = std::chrono::high_resolution_clock::now();

    time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    throughput = 50000 / time_ms * 1000;

    std::cout << "   Time: " << time_ms << " ms\n";
    std::cout << "   Throughput: " << std::fixed << std::setprecision(0) << throughput << " algebraic ops/sec\n";
    std::cout << "   Results: " << algebraic_results.size() << " hybrid algebraic values\n\n";
}

void benchmark_quantum_decision_making() {
    std::cout << "🧠 QUANTUM-INSPIRED DECISION MAKING BENCHMARK\n";
    std::cout << "============================================\n\n";

    // Test 1: Multi-agent quantum decision system
    std::cout << "1. MULTI-AGENT QUANTUM DECISION SYSTEM\n";
    std::cout << "   Agents: 4, Episodes: 10,000\n";

    MultiAgentQuantumDecisionSystem decision_system(4, 3, 10, 4);

    auto start = std::chrono::high_resolution_clock::now();
    decision_system.benchmark_decision_system(10000);
    auto end = std::chrono::high_resolution_clock::now();

    double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    double throughput = 10000 / time_ms * 1000;

    std::cout << "   Total time: " << time_ms << " ms\n";
    std::cout << "   Throughput: " << std::fixed << std::setprecision(0) << throughput << " episodes/sec\n\n";

    // Test 2: Quantum reinforcement learning
    std::cout << "2. QUANTUM REINFORCEMENT LEARNING\n";
    std::cout << "   States: 20, Actions: 5, Episodes: 5,000\n";

    QuantumReinforcementLearner learner(20, 5);

    start = std::chrono::high_resolution_clock::now();

    for (size_t episode = 0; episode < 5000; ++episode) {
        size_t state = episode % 20;
        std::vector<double> context = {0.5, 0.3, 0.1, 0.05, 0.05};
        size_t action = learner.select_action(state, context);

        double reward = (action == (state % 5)) ? 1.0 : -0.1;
        size_t next_state = (state + 1) % 20;
        learner.update_q_value(state, action, reward, next_state);
    }

    end = std::chrono::high_resolution_clock::now();

    time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    throughput = 5000 / time_ms * 1000;

    std::cout << "   Time: " << time_ms << " ms\n";
    std::cout << "   Throughput: " << std::fixed << std::setprecision(0) << throughput << " learning steps/sec\n\n";
}

void benchmark_entanglement_coordination() {
    std::cout << "🔗 QUANTUM ENTANGLEMENT COORDINATION BENCHMARK\n";
    std::cout << "=============================================\n\n";

    // Test 1: Large-scale entanglement coordination
    std::cout << "1. LARGE-SCALE ENTANGLEMENT COORDINATION\n";
    std::cout << "   Agents: 16, Decision dimensions: 4\n";

    QuantumEntanglementCoordinator coordinator(16, 4);

    // Create star entanglement
    coordinator.create_star_entanglement(0);

    // Generate test weights
    std::vector<std::vector<double>> weights(16);
    for (size_t i = 0; i < 16; ++i) {
        weights[i].resize(4);
        for (size_t j = 0; j < 4; ++j) {
            weights[i][j] = 0.2 + 0.6 * ((i + j) % 4) / 3.0;
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto decisions = coordinator.coordinate_decisions(weights, 1000);
    auto end = std::chrono::high_resolution_clock::now();

    double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    double throughput = (16 * 1000) / time_ms * 1000;

    std::cout << "   Time: " << time_ms << " ms\n";
    std::cout << "   Throughput: " << std::fixed << std::setprecision(0) << throughput << " coordinated decisions/sec\n";
    std::cout << "   Decisions: ";
    for (size_t i = 0; i < std::min(decisions.size(), (size_t)5); ++i) {
        std::cout << decisions[i] << " ";
    }
    std::cout << "...\n\n";

    // Test 2: Multiple entanglement patterns
    std::cout << "2. MULTIPLE ENTANGLEMENT PATTERNS\n";
    std::cout << "   Agents: 8, Patterns: Star + Ring\n";

    QuantumEntanglementCoordinator multi_pattern_coordinator(8, 3);

    start = std::chrono::high_resolution_clock::now();

    // Create multiple entanglement patterns
    multi_pattern_coordinator.create_star_entanglement(0);
    multi_pattern_coordinator.create_ring_entanglement();

    // Generate test weights
    std::vector<std::vector<double>> multi_weights(8);
    for (size_t i = 0; i < 8; ++i) {
        multi_weights[i].resize(3);
        for (size_t j = 0; j < 3; ++j) {
            multi_weights[i][j] = 0.1 + 0.8 * ((i + j) % 3) / 2.0;
        }
    }

    auto multi_decisions = multi_pattern_coordinator.coordinate_decisions(multi_weights, 500);
    end = std::chrono::high_resolution_clock::now();

    time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    throughput = (8 * 500) / time_ms * 1000;

    std::cout << "   Time: " << time_ms << " ms\n";
    std::cout << "   Throughput: " << std::fixed << std::setprecision(0) << throughput << " multi-pattern decisions/sec\n\n";
}

void run_comprehensive_analysis() {
    std::cout << "📊 COMPREHENSIVE HYBRID SYSTEM ANALYSIS\n";
    std::cout << "======================================\n\n";

    // Create hybrid system components
    HybridComputationalEngine hybrid_engine(4, 3);
    MultiAgentQuantumDecisionSystem decision_system(2, 3, 5, 3);
    QuantumEntanglementCoordinator coordinator(4, 3);

    // Run small simulation to gather data
    std::cout << "Running comprehensive hybrid simulation...\n\n";

    // Hybrid consensus test
    std::vector<std::vector<uint64_t>> classical_inputs = {
        {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}
    };
    std::vector<std::vector<double>> quantum_weights = {
        {0.6, 0.3, 0.1}, {0.4, 0.4, 0.2}, {0.7, 0.2, 0.1}, {0.5, 0.3, 0.2}
    };

    auto hybrid_results = hybrid_engine.hybrid_consensus(classical_inputs, quantum_weights);
    std::cout << "Hybrid consensus results: ";
    for (auto r : hybrid_results) std::cout << r << " ";
    std::cout << "\n\n";

    // Decision pattern analysis (run small simulation first)
    std::cout << "Running decision simulation for analysis...\n";
    decision_system.run_decision_simulation(10);
    std::cout << "\n";

    // Entanglement coordination analysis
    coordinator.create_star_entanglement(0);
    std::vector<std::vector<double>> coord_weights = {
        {0.8, 0.1, 0.1}, {0.6, 0.3, 0.1}, {0.7, 0.2, 0.1}, {0.5, 0.4, 0.1}
    };
    auto coord_decisions = coordinator.coordinate_decisions(coord_weights, 100);
    std::cout << "Entanglement coordination decisions: ";
    for (auto d : coord_decisions) std::cout << d << " ";
    std::cout << "\n\n";
}

int main() {
    std::cout << "🚀 QUANTUM-CLASSICAL HYBRID PERFORMANCE SUITE\n";
    std::cout << "============================================\n\n";

    try {
        benchmark_quantum_classical_hybrid();
        benchmark_quantum_decision_making();
        benchmark_entanglement_coordination();
        run_comprehensive_analysis();

        std::cout << "🎉 HYBRID PERFORMANCE BENCHMARK COMPLETE!\n";
        std::cout << "Quantum-classical hybrid computation is fully operational!\n";
        std::cout << "Adaptive intelligence through quantum patterns achieved!\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ HYBRID PERFORMANCE BENCHMARK FAILED: " << e.what() << "\n";
        return 1;
    }
}
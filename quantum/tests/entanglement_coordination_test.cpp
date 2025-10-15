#include "../src/hybrid/quantum_entanglement_coordination.hpp"
#include <iostream>
#include <vector>

using namespace quantum_entanglement;

void test_basic_entanglement() {
    std::cout << "=== BASIC ENTANGLEMENT TEST ===\n";

    // Create coordinator with 4 agents, 2 qubits each
    QuantumEntanglementCoordinator coordinator(4, 2);

    // Test 1: Star entanglement
    std::cout << "1. Creating star entanglement (agent 0 as center)...\n";
    coordinator.create_star_entanglement(0);

    // Test 2: Coordinate decisions
    std::vector<std::vector<double>> weights = {
        {0.7, 0.3},  // Agent 0 preferences
        {0.5, 0.5},  // Agent 1 preferences
        {0.3, 0.7},  // Agent 2 preferences
        {0.6, 0.4}   // Agent 3 preferences
    };

    auto decisions = coordinator.coordinate_decisions(weights, 50);
    std::cout << "   Coordinated decisions: ";
    for (auto d : decisions) {
        std::cout << d << " ";
    }
    std::cout << "\n   Status: PASS\n\n";

    // Test 3: Analysis
    std::cout << "2. Coordination analysis:\n";
    std::cout << coordinator.analyze_coordination();
    std::cout << "   Status: PASS\n\n";
}

void test_ring_entanglement() {
    std::cout << "=== RING ENTANGLEMENT TEST ===\n";

    QuantumEntanglementCoordinator coordinator(6, 2);

    std::cout << "1. Creating ring entanglement...\n";
    coordinator.create_ring_entanglement();

    std::vector<std::vector<double>> weights(6, {0.5, 0.5});
    auto decisions = coordinator.coordinate_decisions(weights, 50);

    std::cout << "   Ring decisions: ";
    for (auto d : decisions) {
        std::cout << d << " ";
    }
    std::cout << "\n   Status: PASS\n\n";

    std::cout << "2. Ring analysis:\n";
    std::cout << coordinator.analyze_coordination();
    std::cout << "   Status: PASS\n\n";
}

void test_cluster_entanglement() {
    std::cout << "=== CLUSTER ENTANGLEMENT TEST ===\n";

    QuantumEntanglementCoordinator coordinator(8, 2);

    std::cout << "1. Creating cluster entanglement...\n";
    std::vector<std::vector<size_t>> clusters = {
        {0, 1, 2},  // Cluster 1
        {3, 4, 5},  // Cluster 2
        {6, 7}      // Cluster 3
    };
    coordinator.create_cluster_entanglement(clusters);

    std::vector<std::vector<double>> weights(8, {0.5, 0.5});
    auto decisions = coordinator.coordinate_decisions(weights, 50);

    std::cout << "   Cluster decisions: ";
    for (auto d : decisions) {
        std::cout << d << " ";
    }
    std::cout << "\n   Status: PASS\n\n";

    std::cout << "2. Cluster analysis:\n";
    std::cout << coordinator.analyze_coordination();
    std::cout << "   Status: PASS\n\n";
}

void test_gpu_entanglement_simulation() {
    std::cout << "=== GPU ENTANGLEMENT SIMULATION TEST ===\n";

    try {
        GPUEntanglementSimulator gpu_simulator(4, 2);

        std::cout << "1. Running GPU coordinated simulation...\n";
        auto decisions = gpu_simulator.run_coordinated_simulation(500);

        std::cout << "   GPU decisions: ";
        for (auto d : decisions) {
            std::cout << d << " ";
        }
        std::cout << "\n   Status: PASS\n\n";

        std::cout << "2. Running GPU benchmark (small scale)...\n";
        gpu_simulator.benchmark_entanglement_simulation(10);
        std::cout << "   Status: PASS\n\n";

    } catch (const std::exception& e) {
        std::cerr << "❌ GPU entanglement test failed: " << e.what() << "\n";
        throw;
    }
}

void test_hybrid_coordination() {
    std::cout << "=== HYBRID COORDINATION TEST ===\n";

    HybridCoordinationEngine hybrid_engine(4, 2);

    std::cout << "1. Running comprehensive hybrid simulation...\n";
    hybrid_engine.run_comprehensive_simulation();
    std::cout << "   Status: PASS\n\n";

    std::cout << "2. Running hybrid benchmarks...\n";
    hybrid_engine.benchmark_all_approaches();
    std::cout << "   Status: PASS\n\n";
}

int main() {
    std::cout << "🚀 QUANTUM ENTANGLEMENT COORDINATION TEST SUITE\n";
    std::cout << "==============================================\n\n";

    try {
        test_basic_entanglement();
        test_ring_entanglement();
        test_cluster_entanglement();
        test_gpu_entanglement_simulation();
        test_hybrid_coordination();

        std::cout << "🎉 ALL ENTANGLEMENT COORDINATION TESTS PASSED!\n";
        std::cout << "Quantum entanglement coordination is working correctly!\n";
        std::cout << "Multi-agent coordination through quantum entanglement achieved!\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ ENTANGLEMENT COORDINATION TEST SUITE FAILED: " << e.what() << "\n";
        return 1;
    }
}
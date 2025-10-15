#include "../src/hybrid/quantum_entanglement_coordination_cpu.hpp"
#include <iostream>

using namespace quantum_entanglement_cpu;

void test_minimal_entanglement() {
    std::cout << "=== MINIMAL ENTANGLEMENT TEST ===\n";

    try {
        // Create coordinator with just 2 agents
        std::cout << "1. Creating coordinator with 2 agents...\n";
        QuantumEntanglementCoordinator coordinator(2, 2);
        std::cout << "   Status: PASS\n\n";

        // Test star entanglement
        std::cout << "2. Creating star entanglement...\n";
        coordinator.create_star_entanglement(0);
        std::cout << "   Status: PASS\n\n";

        // Test coordinate decisions
        std::cout << "3. Coordinating decisions...\n";
        std::vector<std::vector<double>> weights = {
            {0.7, 0.3},  // Agent 0 preferences
            {0.5, 0.5}   // Agent 1 preferences
        };

        auto decisions = coordinator.coordinate_decisions(weights, 10);
        std::cout << "   Decisions: ";
        for (auto d : decisions) {
            std::cout << d << " ";
        }
        std::cout << "\n   Status: PASS\n\n";

        std::cout << "✅ MINIMAL ENTANGLEMENT TEST PASSED!\n";

    } catch (const std::exception& e) {
        std::cerr << "❌ Minimal entanglement test failed: " << e.what() << "\n";
        throw;
    }
}

int main() {
    std::cout << "🚀 MINIMAL ENTANGLEMENT TEST\n";
    std::cout << "===========================\n\n";

    try {
        test_minimal_entanglement();
        std::cout << "\n🎉 MINIMAL ENTANGLEMENT TEST COMPLETE!\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ MINIMAL ENTANGLEMENT TEST FAILED: " << e.what() << "\n";
        return 1;
    }
}
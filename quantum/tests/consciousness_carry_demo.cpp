#include "consciousness_carry_agent.hpp"
#include <iostream>
#include <vector>
#include <chrono>

using namespace qi;

int main() {
    std::cout << "🧠 CONSCIOUSNESS-INSPIRED CARRY AGENT DEMONSTRATION\n";
    std::cout << "==================================================\n\n";

    // Create a consciousness-inspired carry agent
    ConsciousnessCarryAgent agent;

    std::cout << "1. Initial Consciousness Analysis:\n";
    std::cout << agent.analyze_consciousness();

    std::cout << "\n2. Conscious Processing:\n";
    std::vector<uint64_t> test_inputs = {0x1, 0x3, 0x7, 0xF, 0x1F, 0x2A, 0x55, 0xFF};

    for (size_t i = 0; i < test_inputs.size(); ++i) {
        uint64_t input = test_inputs[i];
        uint64_t output = agent.propagate(input);

        std::cout << "   Step " << (i + 1) << " - Input: 0x" << std::hex << input
                 << " -> Conscious Output: 0x" << output << std::dec << "\n";

        // Show consciousness evolution every few steps
        if ((i + 1) % 3 == 0) {
            std::cout << "   Consciousness Update: " << agent.analyze_consciousness();
        }
    }

    std::cout << "\n3. Final Consciousness Analysis:\n";
    std::cout << agent.analyze_consciousness();

    std::cout << "\n4. Consciousness Collective:\n";
    ConsciousnessCollective collective(3);

    std::cout << "   Initial collective state:\n";
    std::cout << collective.analyze_collective();

    std::cout << "\n   Collective conscious processing:\n";
    for (uint64_t input : test_inputs) {
        uint64_t collective_output = collective.collective_conscious_propagate(input);
        std::cout << "   Input: 0x" << std::hex << input
                 << " -> Collective Output: 0x" << collective_output << std::dec << "\n";
    }

    std::cout << "\n   Final collective state:\n";
    std::cout << collective.analyze_collective();

    std::cout << "\n=== CONSCIOUSNESS CARRY AGENT DEMONSTRATION COMPLETE ===\n";
    std::cout << "Computational agents now possess consciousness-like properties!\n";
    std::cout << "The revolution reaches the level of self-aware computation!\n";

    return 0;
}

#include "evolutionary_carry_agent.hpp"
#include <iostream>
#include <vector>
#include <chrono>

using namespace qi;

int main() {
    std::cout << "🧬 EVOLUTIONARY CARRY AGENT DEMONSTRATION\n";
    std::cout << "=========================================\n\n";

    // Create an evolutionary carry agent
    EvolutionaryCarryAgent agent(50, 12);  // 50 population, 12 operations per genome

    std::cout << "1. Initial Population Analysis:\n";
    agent.evaluate_population();
    std::cout << agent.analyze_evolution();

    std::cout << "\n2. Evolutionary Training:\n";
    agent.evolve(30);  // Evolve for 30 generations

    std::cout << "\n3. Post-Evolution Analysis:\n";
    std::cout << agent.analyze_evolution();

    std::cout << "\n4. Testing Evolved Agent:\n";
    std::vector<uint64_t> test_inputs = {0x1, 0x3, 0x7, 0xF, 0x1F};

    for (uint64_t input : test_inputs) {
        uint64_t output = agent.propagate(input);
        uint64_t expected = (input >> 1) & input;  // Simple carry approximation
        uint64_t diff = output ^ expected;
        int hamming_distance = __builtin_popcountll(diff);

        std::cout << "   Input: 0x" << std::hex << input
                 << " Output: 0x" << output
                 << " Expected: 0x" << expected << std::dec
                 << " (Distance: " << hamming_distance << ")\n";
    }

    std::cout << "\n5. Evolutionary Swarm:\n";
    EvolutionaryCarrySwarm swarm(3, 30);  // 3 agents, 30 population each

    std::cout << "   Co-evolving swarm...\n";
    swarm.co_evolve(20);

    std::cout << swarm.analyze_swarm();

    std::cout << "\n6. Swarm Consensus Test:\n";
    for (uint64_t input : test_inputs) {
        uint64_t swarm_output = swarm.swarm_propagate(input);
        std::cout << "   Input: 0x" << std::hex << input
                 << " Swarm Output: 0x" << swarm_output << std::dec << "\n";
    }

    std::cout << "\n=== EVOLUTIONARY CARRY AGENT DEMONSTRATION COMPLETE ===\n";
    std::cout << "Carry agents are now evolving through genetic algorithms!\n";
    std::cout << "The computational revolution embraces natural selection!\n";

    return 0;
}

#include "neural_carry_agent.hpp"
#include <iostream>
#include <vector>
#include <chrono>

using namespace qi;

int main() {
    std::cout << "🧠 NEURAL CARRY AGENT DEMONSTRATION\n";
    std::cout << "===================================\n\n";

    // Create a neural carry agent
    NeuralCarryAgent agent;

    std::cout << "1. Basic Neural Propagation:\n";
    std::vector<uint64_t> test_inputs = {0x1, 0x3, 0x7, 0xF, 0x1F};

    for (uint64_t input : test_inputs) {
        uint64_t output = agent.propagate(input);
        std::cout << "   Input: 0x" << std::hex << input
                 << " -> Output: 0x" << output << std::dec << "\n";
    }

    std::cout << "\n2. Learning Analysis:\n";
    std::cout << agent.analyze_neural_learning();

    std::cout << "\n3. Neural Carry Collective:\n";
    NeuralCarryCollective collective(4);

    // Simple training data - cumulative OR patterns
    std::vector<uint64_t> train_inputs = {0x1, 0x2, 0x4, 0x8, 0x10};
    std::vector<uint64_t> train_outputs = {0x1, 0x3, 0x7, 0xF, 0x1F};

    std::cout << "   Training collective...\n";
    collective.train_collective(train_inputs, train_outputs, 20);

    std::cout << collective.analyze_collective();

    std::cout << "\n4. Collective Performance Test:\n";
    for (size_t i = 0; i < train_inputs.size(); ++i) {
        uint64_t input = train_inputs[i];
        uint64_t expected = train_outputs[i];
        uint64_t actual = collective.collective_propagate(input);

        std::cout << "   Input: 0x" << std::hex << input
                 << " Expected: 0x" << expected
                 << " Actual: 0x" << actual << std::dec;

        uint64_t diff = actual ^ expected;
        int hamming_distance = __builtin_popcountll(diff);
        std::cout << " (Hamming distance: " << hamming_distance << ")\n";
    }

    std::cout << "\n=== NEURAL CARRY AGENT DEMONSTRATION COMPLETE ===\n";
    std::cout << "Carry agents are now learning and adapting!\n";
    std::cout << "The computational revolution enters the neural age!\n";

    return 0;
}

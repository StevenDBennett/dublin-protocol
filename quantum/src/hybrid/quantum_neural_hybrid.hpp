#ifndef QUANTUM_NEURAL_HYBRID_HPP
#define QUANTUM_NEURAL_HYBRID_HPP

#include "quantum_cognitive_architecture.hpp"
#include "../agents/neural_carry_agent.hpp"
#include <memory>
#include <chrono>

namespace dublin {

// === QUANTUM-NEURAL HYBRID LEARNING SYSTEM ===
// Combines quantum cognitive architecture with neural carry agents
// for enhanced learning, pattern recognition, and adaptive intelligence

class QuantumNeuralHybridAgent {
private:
    std::unique_ptr<qi::NeuralCarryAgent> neural_agent;
    std::unique_ptr<QuantumCognitiveAgent> quantum_agent;
    std::string hybrid_id;
    std::vector<double> hybrid_performance_history;
    std::mt19937 rng;

    // Hybrid learning parameters
    double neural_weight;
    double quantum_weight;
    double adaptation_rate;

public:
    QuantumNeuralHybridAgent(const std::string& id)
        : hybrid_id(id), rng(std::random_device{}()),
          neural_weight(0.6), quantum_weight(0.4), adaptation_rate(0.1) {

        // Initialize agents directly to avoid move issues
        neural_agent.reset(new qi::NeuralCarryAgent());
        quantum_agent.reset(new QuantumCognitiveAgent(id + "_quantum"));
    }

    // Hybrid learning cycle combining neural and quantum approaches
    uint64_t hybrid_propagate(uint64_t input) {
        // Get predictions from both agents
        uint64_t neural_prediction = neural_agent->propagate(input);
        std::vector<double> quantum_input = {static_cast<double>(input & 0xFFFFFFFF),
                                           static_cast<double>((input >> 32) & 0xFFFFFFFF)};
        std::vector<std::string> context = {"neural_context", "quantum_context"};

        auto quantum_result = quantum_agent->process_cognitive_task(quantum_input, context);

        // Convert quantum result back to uint64_t prediction
        uint64_t quantum_prediction = 0;
        if (!quantum_result.empty()) {
            // Use quantum result to modulate neural prediction
            double quantum_factor = quantum_result[0] * 2.0 - 1.0; // Convert to [-1, 1]
            quantum_prediction = neural_prediction ^ static_cast<uint64_t>(quantum_factor * 1000000);
        }

        // Weighted combination
        uint64_t hybrid_prediction = static_cast<uint64_t>(
            neural_prediction * neural_weight + quantum_prediction * quantum_weight
        );

        return hybrid_prediction;
    }

    // Hybrid learning from experience
    void hybrid_learn(uint64_t input, uint64_t expected_output, bool was_successful) {
        // Learn with neural agent
        neural_agent->learn_from_pattern(input, 0, expected_output, was_successful ? 1.0 : 0.0);

        // Learn with quantum agent
        std::vector<double> quantum_input = {static_cast<double>(input & 0xFFFFFFFF),
                                           static_cast<double>((input >> 32) & 0xFFFFFFFF)};
        std::vector<double> quantum_expected = {static_cast<double>(expected_output & 0xFFFFFFFF),
                                              static_cast<double>((expected_output >> 32) & 0xFFFFFFFF)};
        std::vector<std::string> context = {"learning_context"};

        quantum_agent->learn_from_experience(
            "hybrid_experience_" + std::to_string(input),
            quantum_input, context, was_successful
        );

        // Adapt weights based on performance
        if (was_successful) {
            // Increase weight of better performing component
            if (neural_weight > quantum_weight) {
                neural_weight += adaptation_rate * 0.1;
                quantum_weight -= adaptation_rate * 0.1;
            } else {
                quantum_weight += adaptation_rate * 0.1;
                neural_weight -= adaptation_rate * 0.1;
            }
        }

        // Normalize weights
        double total = neural_weight + quantum_weight;
        neural_weight /= total;
        quantum_weight /= total;

        // Clamp weights
        neural_weight = std::max(0.1, std::min(0.9, neural_weight));
        quantum_weight = std::max(0.1, std::min(0.9, quantum_weight));
    }

    // Hybrid decision making
    std::vector<double> hybrid_decide(const std::vector<uint64_t>& options) {
        std::vector<double> neural_options;
        for (auto opt : options) {
            neural_options.push_back(static_cast<double>(opt));
        }

        // Get quantum decision
        auto quantum_decision = quantum_agent->make_quantum_decision(neural_options);

        // Combine with neural processing
        std::vector<double> hybrid_decision;
        for (size_t i = 0; i < quantum_decision.size(); ++i) {
            double neural_factor = neural_options[i % neural_options.size()] / 1e18; // Normalize
            hybrid_decision.push_back(quantum_decision[i] * quantum_weight +
                                    neural_factor * neural_weight);
        }

        return hybrid_decision;
    }

    // Analyze hybrid performance
    std::string analyze_hybrid_performance() {
        std::stringstream ss;
        ss << "🔄 Quantum-Neural Hybrid Agent '" << hybrid_id << "' Analysis:\n";
        ss << "  Neural weight: " << neural_weight << "\n";
        ss << "  Quantum weight: " << quantum_weight << "\n";
        ss << "  Adaptation rate: " << adaptation_rate << "\n";

        if (!hybrid_performance_history.empty()) {
            double avg_performance = std::accumulate(hybrid_performance_history.begin(),
                                                    hybrid_performance_history.end(), 0.0)
                                   / hybrid_performance_history.size();
            ss << "  Average performance: " << avg_performance << "\n";
        }

        ss << "  Neural agent analysis:\n";
        ss << "    " << neural_agent->analyze_neural_learning() << "\n";

        ss << "  Quantum agent analysis:\n";
        ss << "    " << quantum_agent->analyze_cognitive_state() << "\n";

        return ss.str();
    }

    // Get current weights
    double get_neural_weight() const { return neural_weight; }
    double get_quantum_weight() const { return quantum_weight; }
};

// === QUANTUM-NEURAL HYBRID COLLECTIVE ===
// Multiple hybrid agents working together
class QuantumNeuralHybridCollective {
private:
    std::vector<QuantumNeuralHybridAgent> hybrid_agents;
    std::vector<double> collective_weights;
    std::mt19937 rng;

public:
    QuantumNeuralHybridCollective(size_t num_agents = 3) : rng(std::random_device{}()) {
        collective_weights.resize(num_agents, 1.0 / num_agents);

        for (size_t i = 0; i < num_agents; ++i) {
            hybrid_agents.emplace_back("Hybrid-" + std::to_string(i));
        }
    }

    // Collective hybrid processing
    uint64_t collective_hybrid_process(uint64_t input) {
        std::vector<uint64_t> agent_responses;

        // Get responses from all hybrid agents
        for (auto& agent : hybrid_agents) {
            uint64_t response = agent.hybrid_propagate(input);
            agent_responses.push_back(response);
        }

        // Weighted consensus
        uint64_t collective_result = 0;
        for (size_t i = 0; i < agent_responses.size(); ++i) {
            collective_result += static_cast<uint64_t>(agent_responses[i] * collective_weights[i]);
        }

        return collective_result;
    }

    // Train collective hybrid learning
    void train_collective_hybrid(const std::vector<uint64_t>& training_inputs,
                                const std::vector<uint64_t>& expected_outputs,
                                size_t epochs = 5) {

        for (size_t epoch = 0; epoch < epochs; ++epoch) {
            double epoch_loss = 0.0;

            for (size_t i = 0; i < training_inputs.size(); ++i) {
                uint64_t input = training_inputs[i];
                uint64_t expected = expected_outputs[i];

                // Get collective prediction
                uint64_t actual = collective_hybrid_process(input);

                // Calculate loss (Hamming distance)
                uint64_t diff = actual ^ expected;
                double loss = __builtin_popcountll(diff) / 64.0;
                epoch_loss += loss;

                // Learn with all agents
                for (auto& agent : hybrid_agents) {
                    bool success = (loss < 0.3); // Consider successful if loss < 30%
                    agent.hybrid_learn(input, expected, success);
                }

                // Update collective weights based on individual performance
                for (size_t a = 0; a < hybrid_agents.size(); ++a) {
                    // Reward agents with better neural-quantum balance
                    double neural_w = hybrid_agents[a].get_neural_weight();
                    double quantum_w = hybrid_agents[a].get_quantum_weight();
                    double balance_score = 1.0 - std::abs(neural_w - quantum_w); // Prefer balanced agents

                    if (loss < 0.5) { // Good performance
                        collective_weights[a] *= (1.0 + balance_score * 0.05);
                    } else { // Poor performance
                        collective_weights[a] *= 0.95;
                    }
                }
            }

            // Normalize collective weights
            double weight_sum = std::accumulate(collective_weights.begin(),
                                              collective_weights.end(), 0.0);
            for (auto& w : collective_weights) {
                w /= weight_sum;
            }

            if (epoch % 1 == 0) {
                std::cout << "Hybrid training epoch " << epoch
                         << " - Average loss: " << (epoch_loss / training_inputs.size()) << "\n";
            }
        }
    }

    // Analyze collective hybrid performance
    std::string analyze_collective_hybrid() {
        std::stringstream ss;
        ss << "🌟 Quantum-Neural Hybrid Collective Analysis:\n";
        ss << "  Number of hybrid agents: " << hybrid_agents.size() << "\n";
        ss << "  Collective weights: [";
        for (size_t i = 0; i < collective_weights.size(); ++i) {
            ss << collective_weights[i];
            if (i < collective_weights.size() - 1) ss << ", ";
        }
        ss << "]\n\n";

        ss << "  Individual hybrid agents:\n";
        for (auto& agent : hybrid_agents) {
            ss << "    " << agent.analyze_hybrid_performance() << "\n";
        }

        return ss.str();
    }
};

// === QUANTUM-NEURAL HYBRID DEMONSTRATION ===

class QuantumNeuralHybridDemo {
public:
    static void demonstrate_hybrid_learning() {
        std::cout << "🔄 QUANTUM-NEURAL HYBRID LEARNING SYSTEM DEMONSTRATION\n";
        std::cout << "====================================================\n\n";

        // Create a hybrid agent
        QuantumNeuralHybridAgent hybrid_agent("Demo-Hybrid");

        std::cout << "1. Basic Hybrid Propagation:\n";
        std::vector<uint64_t> test_inputs = {0x123456789ABCDEF0, 0xFEDCBA9876543210, 0x1111111111111111};

        for (uint64_t input : test_inputs) {
            uint64_t result = hybrid_agent.hybrid_propagate(input);
            std::cout << "   Input: 0x" << std::hex << input
                     << " -> Hybrid: 0x" << result << std::dec << "\n";
        }

        std::cout << "\n2. Hybrid Learning:\n";
        // Simple learning examples
        std::vector<std::pair<uint64_t, uint64_t>> learning_examples = {
            {0xAAAAAAAAAAAAAAAA, 0x5555555555555555},
            {0xCCCCCCCCCCCCCCCC, 0x3333333333333333},
            {0xF0F0F0F0F0F0F0F0, 0x0F0F0F0F0F0F0F0F}
        };

        for (auto& example : learning_examples) {
            hybrid_agent.hybrid_learn(example.first, example.second, true);
            std::cout << "   Learned pattern: 0x" << std::hex << example.first
                     << " -> 0x" << example.second << std::dec << "\n";
        }

        std::cout << "\n3. Hybrid Decision Making:\n";
        std::vector<uint64_t> decision_options = {100, 200, 300, 400};
        auto decisions = hybrid_agent.hybrid_decide(decision_options);
        std::cout << "   Decision results: " << decisions.size() << " options evaluated\n";

        std::cout << "\n4. Hybrid Performance Analysis:\n";
        std::cout << hybrid_agent.analyze_hybrid_performance();

        std::cout << "\n5. Quantum-Neural Hybrid Collective:\n";
        QuantumNeuralHybridCollective collective(2);

        // Training data
        std::vector<uint64_t> train_inputs = {0x1111111111111111, 0x2222222222222222, 0x4444444444444444};
        std::vector<uint64_t> train_outputs = {0x8888888888888888, 0x7777777777777777, 0xBBBBBBBBBBBBBBBB};

        std::cout << "   Training collective...\n";
        collective.train_collective_hybrid(train_inputs, train_outputs, 3);

        std::cout << collective.analyze_collective_hybrid();

        std::cout << "\n=== QUANTUM-NEURAL HYBRID LEARNING DEMONSTRATION COMPLETE ===\n";
        std::cout << "Hybrid intelligence combines the best of neural and quantum approaches!\n";
    }
};

} // namespace dublin

#endif // QUANTUM_NEURAL_HYBRID_HPP

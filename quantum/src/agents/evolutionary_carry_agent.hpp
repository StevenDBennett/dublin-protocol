#ifndef EVOLUTIONARY_CARRY_AGENT_HPP
#define EVOLUTIONARY_CARRY_AGENT_HPP

#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>
#include <functional>

namespace qi {

// === EVOLUTIONARY CARRY AGENT: GENETIC ALGORITHMS FOR COMPUTATIONAL PATTERNS ===
// An agent that evolves optimal carry propagation strategies through genetic algorithms

class EvolutionaryCarryAgent {
private:
    // Genome: represents a carry propagation strategy
    struct Genome {
        std::vector<uint8_t> operations;  // Sequence of operations (0=AND, 1=OR, 2=XOR, 3=NAND)
        double fitness;
        size_t age;

        Genome(size_t length = 16) : operations(length), fitness(0.0), age(0) {
            // Initialize with random operations
            std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<uint8_t> op_dist(0, 3);
            for (auto& op : operations) {
                op = op_dist(rng);
            }
        }

        // Apply genome to compute carry
        uint64_t execute(uint64_t input, uint64_t current_carry) const {
            uint64_t result = current_carry;

            for (uint8_t op : operations) {
                switch (op) {
                    case 0: result = (result & input); break;  // AND
                    case 1: result = (result | input); break;  // OR
                    case 2: result = (result ^ input); break;  // XOR
                    case 3: result = ~(result & input); break; // NAND
                }
            }

            return result;
        }

        // Mutate genome
        void mutate(double mutation_rate = 0.1) {
            std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<double> rate_dist(0.0, 1.0);
            std::uniform_int_distribution<uint8_t> op_dist(0, 3);

            for (auto& op : operations) {
                if (rate_dist(rng) < mutation_rate) {
                    op = op_dist(rng);
                }
            }
        }

        // Crossover with another genome
        Genome crossover(const Genome& other) const {
            Genome child;
            std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<size_t> point_dist(1, operations.size() - 1);

            size_t crossover_point = point_dist(rng);

            // Copy from this genome up to crossover point
            for (size_t i = 0; i < crossover_point; ++i) {
                child.operations[i] = operations[i];
            }

            // Copy from other genome after crossover point
            for (size_t i = crossover_point; i < operations.size(); ++i) {
                child.operations[i] = other.operations[i];
            }

            return child;
        }
    };

    // Population of genomes
    std::vector<Genome> population;
    size_t population_size;
    size_t genome_length;
    size_t generation;

    // Evolution parameters
    double mutation_rate;
    double elitism_rate;
    std::mt19937 rng;

    // Fitness evaluation function
    std::function<double(const Genome&)> fitness_function;

public:
    EvolutionaryCarryAgent(size_t pop_size = 100, size_t genome_len = 16)
        : population_size(pop_size), genome_length(genome_len), generation(0),
          mutation_rate(0.1), elitism_rate(0.1), rng(std::random_device{}()) {

        // Initialize population
        population.resize(population_size);
        for (auto& genome : population) {
            genome = Genome(genome_length);
        }

        // Default fitness function: minimize Hamming distance to optimal carry
        fitness_function = [this](const Genome& genome) {
            // Test on various inputs and measure how well it approximates optimal carry
            std::vector<std::pair<uint64_t, uint64_t>> test_cases = {
                {0x1, 0x0}, {0x3, 0x1}, {0x7, 0x3}, {0xF, 0x7}, {0x1F, 0xF},
                {0x2, 0x0}, {0x6, 0x2}, {0xE, 0x6}, {0x1E, 0xE}, {0x3E, 0x1E}
            };

            double total_fitness = 0.0;
            for (const auto& [input, expected_carry] : test_cases) {
                uint64_t actual_carry = genome.execute(input, 0);
                uint64_t diff = actual_carry ^ expected_carry;
                double hamming_distance = __builtin_popcountll(diff) / 64.0;
                total_fitness += (1.0 - hamming_distance);  // Higher fitness for lower distance
            }

            return total_fitness / test_cases.size();
        };
    }

    // Set custom fitness function
    void set_fitness_function(std::function<double(const Genome&)> func) {
        fitness_function = func;
    }

    // Evaluate fitness of entire population
    void evaluate_population() {
        for (auto& genome : population) {
            genome.fitness = fitness_function(genome);
            genome.age++;
        }

        // Sort by fitness (descending)
        std::sort(population.begin(), population.end(),
                 [](const Genome& a, const Genome& b) {
                     return a.fitness > b.fitness;
                 });
    }

    // Create next generation through selection, crossover, and mutation
    void evolve_generation() {
        std::vector<Genome> new_population;

        // Elitism: keep best individuals
        size_t elite_count = static_cast<size_t>(population_size * elitism_rate);
        for (size_t i = 0; i < elite_count; ++i) {
            new_population.push_back(population[i]);
        }

        // Create offspring through tournament selection and crossover
        std::uniform_int_distribution<size_t> tournament_dist(0, population_size - 1);

        while (new_population.size() < population_size) {
            // Tournament selection
            size_t parent1_idx = tournament_dist(rng);
            size_t parent2_idx = tournament_dist(rng);

            const Genome& parent1 = population[parent1_idx];
            const Genome& parent2 = population[parent2_idx];

            // Choose better parent
            const Genome& winner1 = (parent1.fitness > parent2.fitness) ? parent1 : parent2;

            // Second tournament
            parent1_idx = tournament_dist(rng);
            parent2_idx = tournament_dist(rng);
            const Genome& winner2 = (population[parent1_idx].fitness > population[parent2_idx].fitness)
                                  ? population[parent1_idx] : population[parent2_idx];

            // Crossover
            Genome offspring = winner1.crossover(winner2);

            // Mutation
            offspring.mutate(mutation_rate);

            new_population.push_back(offspring);
        }

        population = std::move(new_population);
        generation++;
    }

    // Evolve for multiple generations
    void evolve(size_t generations) {
        for (size_t i = 0; i < generations; ++i) {
            evaluate_population();
            evolve_generation();

            if (i % 10 == 0) {
                std::cout << "Generation " << generation
                         << " - Best fitness: " << population[0].fitness
                         << " - Average fitness: " << get_average_fitness() << "\n";
            }
        }
        evaluate_population();  // Final evaluation
    }

    // Get best genome
    const Genome& get_best_genome() const {
        return population[0];
    }

    // Get average fitness
    double get_average_fitness() const {
        double sum = 0.0;
        for (const auto& genome : population) {
            sum += genome.fitness;
        }
        return sum / population.size();
    }

    // Propagate using best genome
    uint64_t propagate(uint64_t input) const {
        if (population.empty()) return 0;
        return population[0].execute(input, 0);
    }

    // Analyze evolutionary progress
    std::string analyze_evolution() {
        std::stringstream ss;
        ss << "Evolutionary Carry Agent Analysis:\n";
        ss << "  Generation: " << generation << "\n";
        ss << "  Population size: " << population_size << "\n";
        ss << "  Genome length: " << genome_length << "\n";
        ss << "  Best fitness: " << population[0].fitness << "\n";
        ss << "  Average fitness: " << get_average_fitness() << "\n";
        ss << "  Best genome age: " << population[0].age << "\n";

        // Analyze best genome operations
        ss << "  Best genome operations: [";
        for (size_t i = 0; i < population[0].operations.size(); ++i) {
            const char* op_names[] = {"AND", "OR", "XOR", "NAND"};
            ss << op_names[population[0].operations[i]];
            if (i < population[0].operations.size() - 1) ss << ",";
        }
        ss << "]\n";

        return ss.str();
    }
};

// === EVOLUTIONARY CARRY SWARM ===
// Multiple evolutionary agents working together

class EvolutionaryCarrySwarm {
private:
    std::vector<EvolutionaryCarryAgent> agents;
    std::vector<double> agent_fitness_history;
    std::mt19937 rng;

public:
    EvolutionaryCarrySwarm(size_t num_agents = 4, size_t pop_per_agent = 50)
        : rng(std::random_device{}()) {

        agents.resize(num_agents, EvolutionaryCarryAgent(pop_per_agent));
        agent_fitness_history.resize(num_agents, 0.0);
    }

    // Co-evolve all agents
    void co_evolve(size_t generations) {
        for (size_t gen = 0; gen < generations; ++gen) {
            // Each agent evolves independently
            for (auto& agent : agents) {
                agent.evolve(1);
            }

            // Update fitness history
            for (size_t i = 0; i < agents.size(); ++i) {
                agent_fitness_history[i] = agents[i].get_average_fitness();
            }

            // Migration: occasionally swap best genomes between agents
            if (gen % 5 == 0 && agents.size() > 1) {
                migrate_genomes();
            }

            if (gen % 10 == 0) {
                std::cout << "Swarm Generation " << gen
                         << " - Best agent fitness: " << get_best_agent_fitness() << "\n";
            }
        }
    }

    // Migrate best genomes between agents
    void migrate_genomes() {
        std::uniform_int_distribution<size_t> agent_dist(0, agents.size() - 1);

        // Select two random agents
        size_t agent1 = agent_dist(rng);
        size_t agent2 = agent_dist(rng);
        while (agent2 == agent1) agent2 = agent_dist(rng);

        // Note: In a real implementation, we'd need to add methods to access/modify populations
        // For now, this is a placeholder for the migration concept
    }

    // Get swarm consensus
    uint64_t swarm_propagate(uint64_t input) {
        std::vector<uint64_t> outputs;
        for (const auto& agent : agents) {
            outputs.push_back(agent.propagate(input));
        }

        // Majority vote on each bit
        uint64_t consensus = 0;
        for (int bit = 0; bit < 64; ++bit) {
            int votes = 0;
            for (uint64_t output : outputs) {
                if (output & (1ULL << bit)) votes++;
            }
            if (votes > agents.size() / 2) {
                consensus |= (1ULL << bit);
            }
        }

        return consensus;
    }

    double get_best_agent_fitness() const {
        double best = 0.0;
        for (double fitness : agent_fitness_history) {
            best = std::max(best, fitness);
        }
        return best;
    }

    // Analyze swarm performance
    std::string analyze_swarm() {
        std::stringstream ss;
        ss << "Evolutionary Carry Swarm Analysis:\n";
        ss << "  Number of agents: " << agents.size() << "\n";
        ss << "  Best agent fitness: " << get_best_agent_fitness() << "\n";

        double total_fitness = 0.0;
        for (double fitness : agent_fitness_history) {
            total_fitness += fitness;
        }
        ss << "  Average agent fitness: " << (total_fitness / agents.size()) << "\n";

        return ss.str();
    }
};

// === EVOLUTIONARY CARRY AGENT DEMONSTRATION ===

class EvolutionaryCarryDemonstrator {
public:
    static void demonstrate_evolutionary_carry_agent() {
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
    }
};

} // namespace qi

#endif // EVOLUTIONARY_CARRY_AGENT_HPP

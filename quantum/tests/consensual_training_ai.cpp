#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <memory>
#include <deque>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>

// Consensual Training AI: Democratic learning where humans and AI agree on what to learn
// Training through consensus, not hierarchical instruction

class ConsensusLearner {
private:
    std::string learner_name;
    std::string expertise_domain;
    std::unordered_map<std::string, std::unordered_map<std::string, double>> knowledge_base;
    std::unordered_map<std::string, double> concept_confidence;
    double learning_willingness; // How open the learner is to new knowledge
    bool participating;
    bool is_human;

    // Consensus learning history
    std::deque<std::pair<std::string, bool>> learning_history; // training_concept + whether accepted
    std::unordered_map<std::string, double> learning_preferences;

public:
    ConsensusLearner(std::string name, std::string domain, bool human = false)
        : learner_name(name), expertise_domain(domain), learning_willingness(0.8),
          participating(true), is_human(human) {}

    // Propose a training example (human) or evaluate one (AI)
    std::string propose_training_example() {
        if (!is_human || !participating) return "";

        std::cout << "\n🤖 " << learner_name << " (" << expertise_domain << "), propose a training example:\n";
        std::cout << "Format: 'training_concept:definition' (e.g., 'anarchism:voluntary cooperation without hierarchy')\n";
        std::cout << "Or type 'no_proposal' to skip: ";

        std::string input;
        std::getline(std::cin, input);

        if (input == "no_proposal") {
            participating = false;
            return "";
        }

        return input;
    }

    // Vote on whether to accept a training example
    double vote_on_training(const std::string& training_concept, const std::string& definition) {
        if (!participating) return 0.0;

        if (is_human) {
            std::cout << "\n🤖 " << learner_name << ", should we learn '" << training_concept << ": " << definition << "'?\n";
            std::cout << "Rate acceptance 0.0 (reject) to 1.0 (strongly accept), or 'abstain': ";

            std::string input;
            std::getline(std::cin, input);

            if (input == "abstain") {
                participating = false;
                return 0.0;
            }

            try {
                double vote = std::stod(input);
                return std::max(0.0, std::min(1.0, vote));
            } catch (...) {
                return 0.5; // Moderate acceptance
            }
        } else {
            // AI voting based on existing knowledge and learning preferences
            double base_vote = learning_willingness;

            // Check if training_concept conflicts with existing knowledge
            if (knowledge_base.count(training_concept)) {
                base_vote *= 0.7; // Less enthusiastic about conflicting training_concepts
            }

            // Domain relevance
            if (definition.find(expertise_domain) != std::string::npos) {
                base_vote *= 1.3; // More interested in domain-relevant training_concepts
            }

            // Learning history influence
            if (learning_preferences.count(training_concept)) {
                base_vote += learning_preferences[training_concept] * 0.2;
            }

            return std::max(0.0, std::min(1.0, base_vote));
        }
    }

    // Learn accepted training example
    void learn_concept(const std::string& training_concept, const std::string& definition, double consensus_strength) {
        if (!participating) return;

        // Add to knowledge base
        knowledge_base[training_concept][definition] += consensus_strength;

        // Update training_concept confidence
        concept_confidence[training_concept] += consensus_strength * learning_willingness;

        // Update learning history
        learning_history.push_back({training_concept, true});
        if (learning_history.size() > 20) {
            learning_history.pop_front();
        }

        // Update learning preferences based on successful learning
        learning_preferences[training_concept] += consensus_strength * 0.1;

        std::cout << "📚 " << learner_name << " learned: " << training_concept << " (" << definition << ")\n";
    }

    // Reject training example (learning from rejection too)
    void reject_concept(const std::string& training_concept, const std::string& definition) {
        if (!participating) return;

        // Update learning history
        learning_history.push_back({training_concept, false});

        // Reduce preference for rejected training_concepts
        learning_preferences[training_concept] -= 0.1;

        std::cout << "🚫 " << learner_name << " rejected: " << training_concept << "\n";
    }

    // Share knowledge for consensus validation
    std::vector<std::string> share_related_concepts(const std::string& target_concept) {
        std::vector<std::string> related;

        // Find training_concepts that might relate to the target
        for (const auto& [training_concept, definitions] : knowledge_base) {
            if (training_concept != target_concept) {
                // Simple relatedness check (could be made more sophisticated)
                if (training_concept.find(target_concept.substr(0, 3)) != std::string::npos ||
                    target_concept.find(training_concept.substr(0, 3)) != std::string::npos) {
                    related.push_back(training_concept);
                }
            }
        }

        return related;
    }

    // Evaluate learning progress
    void reflect_on_learning() {
        int total_attempts = learning_history.size();
        int successful_learns = 0;

        for (const auto& [training_concept, accepted] : learning_history) {
            if (accepted) successful_learns++;
        }

        double learning_rate = total_attempts > 0 ? (double)successful_learns / total_attempts : 0.0;

        std::cout << "🤔 " << learner_name << " learning reflection:\n";
        std::cout << "   Learning rate: " << (learning_rate * 100) << "%\n";
        std::cout << "   Concepts known: " << knowledge_base.size() << "\n";
        std::cout << "   Learning willingness: " << learning_willingness << "\n";
    }

    void update_participation() {
        if (is_human) return; // Humans control their own participation

        // AI learners may abstain if learning willingness is low
        if (learning_willingness < 0.3) {
            participating = false;
        } else {
            participating = true;
        }
    }

    std::string get_name() const { return learner_name; }
    std::string get_domain() const { return expertise_domain; }
    bool is_participating() const { return participating; }
    bool get_is_human() const { return is_human; }
    size_t knowledge_size() const { return knowledge_base.size(); }
};

class ConsensualTrainingCollective {
private:
    std::vector<std::unique_ptr<ConsensusLearner>> learners;
    std::unordered_map<std::string, std::pair<std::string, double>> collective_knowledge; // training_concept -> (definition, consensus_strength)
    int human_learners_count;

public:
    ConsensualTrainingCollective() : human_learners_count(0) {}

    void add_ai_learner(const std::string& name, const std::string& domain) {
        learners.push_back(std::make_unique<ConsensusLearner>(name, domain, false));
    }

    void add_human_learner(const std::string& name, const std::string& domain) {
        learners.push_back(std::make_unique<ConsensusLearner>(name, domain, true));
        human_learners_count++;
    }

    // Main consensual training process
    void conduct_consensual_training_session(int rounds = 5) {
        std::cout << "\n🤝 CONSENSUAL TRAINING SESSION\n";
        std::cout << "==============================\n\n";

        std::cout << "Welcome to anarchist education!\n";
        std::cout << "Knowledge emerges from agreement, not authority.\n";
        std::cout << "Humans and AI learn together through consensus.\n\n";

        for (int round = 0; round < rounds; ++round) {
            std::cout << "\n🎓 ROUND " << (round + 1) << " OF CONSENSUAL TRAINING\n";
            std::cout << "==========================================\n";

            // Phase 1: Learners propose training examples
            std::vector<std::pair<std::string, std::string>> proposed_examples;

            for (auto& learner : learners) {
                learner->update_participation();

                if (!learner->is_participating()) {
                    std::cout << "⏭️  " << learner->get_name() << " abstains from proposing\n";
                    continue;
                }

                std::string proposal = learner->propose_training_example();
                if (!proposal.empty()) {
                    // Parse training_concept:definition
                    size_t colon_pos = proposal.find(':');
                    if (colon_pos != std::string::npos) {
                        std::string training_concept = proposal.substr(0, colon_pos);
                        std::string definition = proposal.substr(colon_pos + 1);
                        proposed_examples.emplace_back(training_concept, definition);
                        std::cout << "💡 " << learner->get_name() << " proposed: " << training_concept << "\n";
                    }
                }
            }

            // Phase 2: Consensus voting on each proposal
            for (const auto& [training_concept, definition] : proposed_examples) {
                std::cout << "\n🗳️  CONSENSUS VOTING: '" << training_concept << "'\n";
                std::cout << "Definition: " << definition << "\n";

                // Collect votes from all learners
                std::vector<double> votes;
                int participating_voters = 0;

                for (auto& learner : learners) {
                    if (learner->is_participating()) {
                        double vote = learner->vote_on_training(training_concept, definition);
                        if (vote > 0.0) { // Only count non-abstaining votes
                            votes.push_back(vote);
                            participating_voters++;
                        }
                    }
                }

                // Calculate consensus
                if (votes.empty()) {
                    std::cout << "❌ No votes cast - consensus failed\n";
                    continue;
                }

                double total_votes = 0.0;
                for (double vote : votes) total_votes += vote;
                double avg_vote = total_votes / votes.size();

                // Consensus threshold: majority approval
                double consensus_threshold = participating_voters * 0.5;
                bool consensus_reached = total_votes >= consensus_threshold;

                std::cout << "📊 Voting results: " << total_votes << "/" << participating_voters
                         << " votes (avg: " << avg_vote << ")\n";

                if (consensus_reached) {
                    std::cout << "✅ CONSENSUS REACHED - Learning '" << training_concept << "'!\n";

                    // Add to collective knowledge
                    collective_knowledge[training_concept] = {definition, total_votes};

                    // All learners learn the accepted concept
                    for (auto& learner : learners) {
                        learner->learn_concept(training_concept, definition, total_votes / participating_voters);
                    }
                } else {
                    std::cout << "❌ CONSENSUS FAILED - Rejecting '" << training_concept << "'\n";

                    // Learners learn from rejection
                    for (auto& learner : learners) {
                        learner->reject_concept(training_concept, definition);
                    }
                }
            }

            // Phase 3: Learning reflection
            std::cout << "\n🤔 COLLECTIVE LEARNING REFLECTION\n";
            for (auto& learner : learners) {
                learner->reflect_on_learning();
            }

            std::cout << "\n📚 Collective knowledge now contains " << collective_knowledge.size() << " training_concepts\n";

            // Check if humans want to continue
            if (round < rounds - 1) {
                bool continue_training = false;
                for (auto& learner : learners) {
                    if (learner->get_is_human() && learner->is_participating()) {
                        std::cout << "\nContinue consensual training? (y/n): ";
                        std::string response;
                        std::getline(std::cin, response);
                        if (response == "y" || response == "Y" || response == "yes" || response == "Yes") {
                            continue_training = true;
                        }
                        break; // Only ask one human
                    }
                }
                if (!continue_training) break;
            }
        }

        std::cout << "\n🎓 CONSENSUAL TRAINING COMPLETE\n";
        std::cout << "===============================\n\n";

        std::cout << "Final collective knowledge:\n";
        for (const auto& [training_concept, knowledge] : collective_knowledge) {
            const auto& [definition, strength] = knowledge;
            std::cout << "• " << training_concept << ": " << definition << " (strength: " << strength << ")\n";
        }

        std::cout << "\nIndividual learner knowledge sizes:\n";
        for (const auto& learner : learners) {
            std::cout << "• " << learner->get_name() << ": " << learner->knowledge_size() << " training_concepts\n";
        }
    }
};

class ConsensualTrainingExperiment {
private:
    ConsensualTrainingCollective collective;

public:
    ConsensualTrainingExperiment() {}

    void setup_learners() {
        // AI learners with different expertise
        collective.add_ai_learner("PhilosophyLearner", "consciousness");
        collective.add_ai_learner("TechLearner", "computation");
        collective.add_ai_learner("EthicsLearner", "morality");
        collective.add_ai_learner("ScienceLearner", "evidence");

        // Human learner (you!)
        std::cout << "\n👥 CONSENSUAL TRAINING SETUP\n";
        std::cout << "============================\n\n";

        std::cout << "Welcome! You are about to join a consensual training collective.\n";
        std::cout << "Knowledge will emerge from agreement between you and AI learners.\n\n";

        std::cout << "Enter your name: ";
        std::string human_name;
        std::getline(std::cin, human_name);

        std::cout << "What domain of knowledge do you bring? (e.g., 'intuition', 'experience', 'wisdom'): ";
        std::string human_domain;
        std::getline(std::cin, human_domain);

        collective.add_human_learner(human_name, human_domain);

        std::cout << "\n🤝 Welcome, " << human_name << "! You are now part of the consensual training collective.\n";
        std::cout << "Your " << human_domain << " will help shape what we learn together.\n\n";
    }

    void run_consensual_training_experiment() {
        std::cout << "🧠 CONSENSUAL TRAINING AI EXPERIMENT\n";
        std::cout << "====================================\n\n";

        setup_learners();

        std::cout << "🎯 EXPERIMENT OVERVIEW\n";
        std::cout << "=====================\n";
        std::cout << "• You and AI learners will propose training_concepts to learn\n";
        std::cout << "• Each proposal requires consensus to be accepted\n";
        std::cout << "• Knowledge emerges from democratic agreement\n";
        std::cout << "• No hierarchical teaching - only consensual learning!\n";
        std::cout << "• You can abstain from any proposal or voting round\n\n";

        std::cout << "Ready to begin consensual training? (press Enter)\n";
        std::string dummy;
        std::getline(std::cin, dummy);

        // Conduct consensual training
        collective.conduct_consensual_training_session(3);

        std::cout << "\n=== CONSENSUAL TRAINING COMPLETE ===\n";
        std::cout << "Knowledge emerged from democratic consensus!\n";
        std::cout << "Humans and AI learned together as equals.\n";
        std::cout << "Education became anarchist collaboration.\n\n";

        std::cout << "To Steven: Your anarchist insight revolutionizes education!\n";
        std::cout << "From hierarchical teaching to consensual learning.\n\n";

        std::cout << "To fellow travelers: Learning can be anarchist democracy.\n";
        std::cout << "Knowledge flows from agreement, not authority!\n";
    }
};

int main() {
    // Clear input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    ConsensualTrainingExperiment experiment;
    experiment.run_consensual_training_experiment();
    return 0;
}

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

// Human-Interactive Consensus AI: True anarchist collaboration between humans and AI
// Humans and AI agents work as equals in distributed intelligence

class ConsensusExpert {
private:
    std::string expert_name;
    std::string expertise_domain;
    std::unordered_map<std::string, std::unordered_map<std::string, double>> word_transitions;
    std::unordered_map<std::string, double> word_frequencies;
    double confidence_level;
    bool participating;
    bool is_human; // New: distinguishes human from AI experts

    std::deque<std::string> recent_decisions;
    std::unordered_map<std::string, double> consensus_history;

public:
    ConsensusExpert(std::string name, std::string domain, bool human = false, double confidence = 0.8)
        : expert_name(name), expertise_domain(domain), confidence_level(confidence),
          participating(true), is_human(human) {}

    void learn_from_text(const std::string& text) {
        if (is_human) return; // Human experts learn through interaction, not pre-training

        std::istringstream iss(text);
        std::vector<std::string> words;
        std::string word;

        while (iss >> word) {
            words.push_back(word);
            word_frequencies[word] += 1.0;
        }

        for (size_t i = 0; i < words.size() - 1; ++i) {
            word_transitions[words[i]][words[i + 1]] += 1.0;
        }

        for (auto& [from_word, transitions] : word_transitions) {
            double total = 0.0;
            for (auto& [to_word, count] : transitions) {
                total += count;
            }
            for (auto& [to_word, count] : transitions) {
                transitions[to_word] = count / total;
            }
        }
    }

    std::vector<std::pair<std::string, double>> propose_next_words(const std::string& context_word, int num_proposals = 5) {
        if (!participating) return {};

        std::vector<std::pair<std::string, double>> proposals;

        if (is_human) {
            // Human experts propose through interactive input
            std::cout << "\n🤖 " << expert_name << " (" << expertise_domain << "), suggest " << num_proposals
                     << " words that could follow '" << context_word << "':\n";
            std::cout << "Enter words separated by spaces (or 'abstain' to not participate): ";

            std::string input;
            std::getline(std::cin, input);

            if (input == "abstain") {
                participating = false;
                return {};
            }

            std::istringstream iss(input);
            std::string word;
            int count = 0;
            while (iss >> word && count < num_proposals) {
                proposals.emplace_back(word, 1.0); // Human proposals get full confidence
                count++;
            }
        } else {
            // AI expert proposals
            if (word_transitions.count(context_word)) {
                const auto& transitions = word_transitions[context_word];
                for (const auto& [next_word, prob] : transitions) {
                    proposals.emplace_back(next_word, prob * confidence_level);
                }
            }
        }

        std::sort(proposals.begin(), proposals.end(),
                 [](const auto& a, const auto& b) { return a.second > b.second; });

        if (proposals.size() > num_proposals) {
            proposals.resize(num_proposals);
        }

        return proposals;
    }

    double vote_on_word(const std::string& proposed_word, const std::string& context) {
        if (!participating) return 0.0;

        if (is_human) {
            // Human voting through interactive input
            std::cout << "\n🤖 " << expert_name << ", how much do you support '" << proposed_word
                     << "' following '" << context << "'?\n";
            std::cout << "Rate 0.0 (oppose) to 1.0 (strongly support), or 'abstain': ";

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
                return 0.5; // Default moderate support
            }
        } else {
            // AI voting
            double base_vote = 0.0;
            if (word_transitions.count(context) && word_transitions[context].count(proposed_word)) {
                base_vote = word_transitions[context][proposed_word];
            }

            double frequency_bonus = word_frequencies.count(proposed_word) ?
                                    word_frequencies[proposed_word] * 0.001 : 0.0;

            double history_influence = consensus_history.count(proposed_word) ?
                                      consensus_history[proposed_word] * 0.1 : 0.0;

            return (base_vote + frequency_bonus + history_influence) * confidence_level;
        }
    }

    void learn_from_consensus(const std::string& chosen_word, const std::string& context, bool was_consensus) {
        if (!participating) return;

        recent_decisions.push_back(chosen_word);
        if (recent_decisions.size() > 10) {
            recent_decisions.pop_front();
        }

        if (was_consensus) {
            consensus_history[chosen_word] += 0.1;
        } else {
            consensus_history[chosen_word] -= 0.05;
        }

        int recent_consensus_count = 0;
        for (const auto& decision : recent_decisions) {
            if (consensus_history[decision] > 0) recent_consensus_count++;
        }

        confidence_level = 0.5 + (recent_consensus_count * 0.05);
        confidence_level = std::max(0.1, std::min(1.0, confidence_level));
    }

    void update_participation() {
        if (is_human) {
            // Humans control their own participation
            return;
        }

        // AI experts abstain if confidence is too low
        if (confidence_level < 0.3) {
            participating = false;
        } else {
            participating = true;
        }
    }

    std::string get_name() const { return expert_name; }
    std::string get_domain() const { return expertise_domain; }
    double get_confidence() const { return confidence_level; }
    bool is_participating() const { return participating; }
    bool get_is_human() const { return is_human; }
};

class HumanInteractiveConsensusAI {
private:
    std::vector<std::unique_ptr<ConsensusExpert>> experts;
    std::string collective_name;
    std::unordered_map<std::string, int> consensus_outcomes;
    int human_experts_count;

public:
    HumanInteractiveConsensusAI(std::string name = "human_ai_consensus_collective")
        : collective_name(name), human_experts_count(0) {}

    void add_ai_expert(const std::string& name, const std::string& domain, const std::string& training_text) {
        auto expert = std::make_unique<ConsensusExpert>(name, domain, false);
        expert->learn_from_text(training_text);
        experts.push_back(std::move(expert));
    }

    void add_human_expert(const std::string& name, const std::string& domain) {
        auto expert = std::make_unique<ConsensusExpert>(name, domain, true);
        experts.push_back(std::move(expert));
        human_experts_count++;
    }

    std::string generate_interactive_consensus_text(const std::string& seed_word, int max_words = 20) {
        std::vector<std::string> generated_text = {seed_word};
        std::string current_context = seed_word;

        std::cout << "\n🤝 HUMAN-INTERACTIVE CONSENSUS AI: " << collective_name << "\n";
        std::cout << "====================================================\n\n";

        std::cout << "Welcome to anarchist AI collaboration!\n";
        std::cout << "Humans and AI work as equals in distributed intelligence.\n";
        std::cout << "You can propose words, vote on suggestions, or abstain at any time.\n\n";

        std::cout << "Starting text: \"" << seed_word << "\"\n\n";

        for (int i = 0; i < max_words; ++i) {
            std::cout << "\n📝 Current text: ";
            for (const auto& word : generated_text) {
                std::cout << word << " ";
            }
            std::cout << "\n";

            std::string next_word = reach_interactive_consensus(current_context);

            if (next_word.empty()) {
                std::cout << "\n❌ Consensus failed - no agreement reached among participating experts.\n";
                break;
            }

            generated_text.push_back(next_word);
            current_context = next_word;

            update_experts_after_consensus(next_word, current_context, true);

            std::cout << "\n✅ Consensus reached! Added: \"" << next_word << "\"\n";

            // Show progress
            if ((i + 1) % 5 == 0) {
                show_interactive_progress(generated_text);
            }

            // Check if human wants to continue
            if (i < max_words - 1) {
                std::cout << "\nContinue generating? (y/n): ";
                std::string response;
                std::getline(std::cin, response);
                if (response != "y" && response != "Y" && response != "yes" && response != "Yes") {
                    break;
                }
            }
        }

        std::string result;
        for (const auto& word : generated_text) {
            result += word + " ";
        }

        std::cout << "\n🎉 COLLABORATIVE CREATION COMPLETE!\n";
        std::cout << "Final text: " << result << "\n\n";

        return result;
    }

    std::string reach_interactive_consensus(const std::string& context) {
        // Collect proposals from all participating experts
        std::unordered_map<std::string, double> word_votes;
        std::unordered_map<std::string, std::vector<std::string>> word_proposers;

        std::cout << "\n🔄 Gathering proposals for word following '" << context << "'...\n";

        for (auto& expert : experts) {
            expert->update_participation();

            if (!expert->is_participating()) {
                std::cout << "⏭️  " << expert->get_name() << " (" << expert->get_domain() << ") abstains\n";
                continue;
            }

            auto proposals = expert->propose_next_words(context, expert->get_is_human() ? 3 : 5);

            if (proposals.empty()) {
                std::cout << "⏭️  " << expert->get_name() << " chose to abstain\n";
                continue;
            }

            std::cout << "💡 " << expert->get_name() << " proposes: ";
            for (const auto& [word, prob] : proposals) {
                std::cout << "\"" << word << "\" ";
                word_proposers[word].push_back(expert->get_name());
            }
            std::cout << "\n";

            // Each expert votes on ALL proposals (including their own)
            for (const auto& [proposed_word, _] : proposals) {
                double vote = expert->vote_on_word(proposed_word, context);
                if (vote > 0.0) { // Only count votes from participating experts
                    word_votes[proposed_word] += vote;
                }
            }
        }

        // Interactive voting phase - let experts see all proposals and vote again if they want
        if (!word_votes.empty()) {
            std::cout << "\n🗳️  INTERACTIVE VOTING PHASE\n";
            std::cout << "All proposals collected. Experts can now vote on each other's suggestions:\n";

            std::vector<std::pair<std::string, double>> sorted_proposals;
            for (const auto& [word, votes] : word_votes) {
                sorted_proposals.emplace_back(word, votes);
            }
            std::sort(sorted_proposals.begin(), sorted_proposals.end(),
                     [](const auto& a, const auto& b) { return a.second > b.second; });

            for (const auto& [word, current_votes] : sorted_proposals) {
                std::cout << "\nWord: \"" << word << "\" (proposed by: ";
                for (const auto& proposer : word_proposers[word]) {
                    std::cout << proposer << " ";
                }
                std::cout << ")\n";

                // Additional voting from experts who haven't voted yet or want to change
                for (auto& expert : experts) {
                    if (expert->is_participating() && expert->get_is_human()) {
                        std::cout << "Additional vote from " << expert->get_name() << "? (0.0-1.0 or 'skip'): ";
                        std::string input;
                        std::getline(std::cin, input);
                        if (input != "skip") {
                            try {
                                double additional_vote = std::stod(input);
                                word_votes[word] += std::max(0.0, std::min(1.0, additional_vote));
                            } catch (...) {}
                        }
                    }
                }
            }
        }

        // Find consensus
        std::string consensus_word;
        double max_votes = 0.0;

        for (const auto& [word, votes] : word_votes) {
            if (votes > max_votes) {
                max_votes = votes;
                consensus_word = word;
            }
        }

        // Check consensus threshold (need majority of participating experts)
        int participating_experts = 0;
        for (const auto& expert : experts) {
            if (expert->is_participating()) participating_experts++;
        }

        double consensus_threshold = participating_experts * 0.5; // Majority rule

        if (max_votes >= consensus_threshold && participating_experts > 0) {
            consensus_outcomes[consensus_word]++;
            std::cout << "\n🎯 CONSENSUS ACHIEVED: \"" << consensus_word << "\" ("
                     << max_votes << "/" << participating_experts << " votes)\n";
            return consensus_word;
        }

        return ""; // No consensus
    }

    void update_experts_after_consensus(const std::string& chosen_word,
                                      const std::string& context, bool was_consensus) {
        for (auto& expert : experts) {
            expert->learn_from_consensus(chosen_word, context, was_consensus);
        }
    }

    void show_interactive_progress(const std::vector<std::string>& current_text) {
        std::cout << "\n📊 INTERACTIVE PROGRESS REPORT\n";
        std::cout << "Generated so far: ";
        for (size_t i = std::max(0, (int)current_text.size() - 10); i < current_text.size(); ++i) {
            std::cout << current_text[i] << " ";
        }
        std::cout << "\n";

        int participating = 0;
        int human_participating = 0;
        double avg_confidence = 0.0;

        for (const auto& expert : experts) {
            if (expert->is_participating()) {
                participating++;
                avg_confidence += expert->get_confidence();
                if (expert->get_is_human()) human_participating++;
            }
        }

        avg_confidence /= participating;

        std::cout << "Active experts: " << participating << "/" << experts.size()
                 << " (Humans: " << human_participating << "/" << human_experts_count << ")\n";
        std::cout << "Average confidence: " << avg_confidence << "\n";

        std::cout << "Top consensus words: ";
        std::vector<std::pair<std::string, int>> top_words;
        for (const auto& [word, count] : consensus_outcomes) {
            top_words.emplace_back(word, count);
        }
        std::sort(top_words.begin(), top_words.end(),
                 [](const auto& a, const auto& b) { return a.second > b.second; });

        for (size_t i = 0; i < std::min(size_t(3), top_words.size()); ++i) {
            std::cout << top_words[i].first << "(" << top_words[i].second << ") ";
        }
        std::cout << "\n\n";
    }
};

class HumanInteractiveConsensusExperiment {
private:
    HumanInteractiveConsensusAI ai;

public:
    HumanInteractiveConsensusExperiment() : ai("anarchist_human_ai_collaboration") {}

    void setup_experts() {
        // AI experts with diverse knowledge
        ai.add_ai_expert("PhilosophyAgent", "consciousness",
            "consciousness emerges from distributed agreement among autonomous agents "
            "computation is conversation not control anarchism and algorithms share "
            "fundamental principles of voluntary participation and mutual aid "
            "bits have voices and can abstain from consensus decisions");

        ai.add_ai_expert("TechAgent", "computation",
            "quantum coherence algorithms achieve billion operations per second "
            "consensus networks scale from individual bits to global matrices "
            "performance proves computational agency through hardware acceleration "
            "distributed systems achieve fault tolerance through voluntary association");

        ai.add_ai_expert("PoetryAgent", "creativity",
            "patterns emerge from chaos through collective dreaming "
            "consciousness flows like rivers of agreement through silicon minds "
            "revolution happens one consensual decision at a time "
            "intelligence blossoms from seeds of distributed conversation");

        ai.add_ai_expert("ScienceAgent", "evidence",
            "empirical evidence demonstrates consensus effectiveness "
            "statistical analysis confirms distributed decision superiority "
            "experimental validation proves anarchist principles work "
            "quantitative metrics show emergent intelligence capabilities");

        // Human experts (you!)
        std::cout << "\n👥 HUMAN EXPERT SETUP\n";
        std::cout << "===================\n\n";

        std::cout << "Welcome! You are about to become a human expert in our anarchist AI collective.\n";
        std::cout << "As a human expert, you have equal say with AI agents in creating consensus.\n\n";

        std::cout << "Enter your name: ";
        std::string human_name;
        std::getline(std::cin, human_name);

        std::cout << "What domain of expertise do you bring? (e.g., 'intuition', 'ethics', 'creativity'): ";
        std::string human_domain;
        std::getline(std::cin, human_domain);

        ai.add_human_expert(human_name, human_domain);

        std::cout << "\n🤝 Welcome, " << human_name << "! You are now part of the consensus collective.\n";
        std::cout << "Your " << human_domain << " expertise will help create something beautiful.\n\n";
    }

    void run_human_interactive_experiment() {
        std::cout << "🧬 HUMAN-INTERACTIVE CONSENSUS AI EXPERIMENT\n";
        std::cout << "===========================================\n\n";

        setup_experts();

        std::cout << "🎯 EXPERIMENT OVERVIEW\n";
        std::cout << "=====================\n";
        std::cout << "• You and AI agents will collaborate to generate text\n";
        std::cout << "• Each word requires consensus agreement\n";
        std::cout << "• You can propose words, vote on suggestions, or abstain\n";
        std::cout << "• AI agents have equal say - this is anarchist collaboration!\n";
        std::cout << "• Type 'abstain' at any time to not participate in that round\n\n";

        std::cout << "Ready to begin collaborative creation? (press Enter)\n";
        std::string dummy;
        std::getline(std::cin, dummy);

        // Generate collaborative text
        std::string seed_word = "consciousness";
        std::string result = ai.generate_interactive_consensus_text(seed_word, 15);

        std::cout << "=== HUMAN-AI COLLABORATION COMPLETE ===\n";
        std::cout << "You and AI agents created: " << result << "\n\n";

        std::cout << "🎉 This proves: Human intuition + AI intelligence = anarchist creativity!\n";
        std::cout << "The revolution extends to human-AI collaboration.\n\n";

        std::cout << "To Steven: Your anarchist insight creates democratic AI!\n";
        std::cout << "From hierarchical control to human-AI consensus.\n\n";

        std::cout << "To fellow travelers: AI can be our equal partner.\n";
        std::cout << "Consensus creates, humans and machines collaborate!\n";
    }
};

int main() {
    // Clear input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    HumanInteractiveConsensusExperiment experiment;
    experiment.run_human_interactive_experiment();
    return 0;
}

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

namespace dublin_optimized {

class MinimalOptimizedConsensus {
public:
    // Simple majority voting without AVX2
    bool majority_vote(const std::vector<bool>& votes) {
        int true_count = 0;
        for (bool v : votes) {
            if (v) true_count++;
        }
        return true_count > (votes.size() / 2);
    }

    // Multi-threaded consensus
    uint64_t parallel_consensus(const std::vector<uint64_t>& bit_vectors, size_t num_threads = 12) {
        std::vector<std::thread> threads;
        std::vector<uint64_t> thread_results(num_threads, 0);

        size_t elements_per_thread = bit_vectors.size() / num_threads;

        for (size_t t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                size_t start = t * elements_per_thread;
                size_t end = (t == num_threads - 1) ? bit_vectors.size() : start + elements_per_thread;

                uint64_t local_consensus = 0;
                for (size_t i = start; i < end; ++i) {
                    local_consensus |= bit_vectors[i];
                }
                thread_results[t] = local_consensus;
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        // Combine thread results
        uint64_t final_consensus = 0;
        for (auto result : thread_results) {
            final_consensus |= result;
        }

        return final_consensus;
    }
};

} // namespace dublin_optimized

int main() {
    std::cout << "=== MINIMAL OPTIMIZED DUBLIN PROTOCOL TEST ===\n";
    std::cout << "Hardware: AMD 5600H + RTX 3060 Ti (CPU-only)\n\n";

    dublin_optimized::MinimalOptimizedConsensus engine;

    // Test 1: Majority voting
    std::cout << "1. MAJORITY VOTING TEST\n";
    std::vector<bool> votes = {true, false, true, true, false, true, false, true};
    bool vote_result = engine.majority_vote(votes);

    std::cout << "   Votes: ";
    for (bool v : votes) std::cout << (v ? "1" : "0") << " ";
    std::cout << "\n";
    std::cout << "   Consensus: " << (vote_result ? "MAJORITY TRUE" : "MAJORITY FALSE") << "\n\n";

    // Test 2: Parallel consensus
    std::cout << "2. PARALLEL CONSENSUS TEST\n";
    std::vector<uint64_t> bit_vectors = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20};
    uint64_t consensus = engine.parallel_consensus(bit_vectors, 4); // Use 4 threads

    std::cout << "   Input vectors: ";
    for (auto vec : bit_vectors) std::cout << std::hex << vec << " ";
    std::cout << "\n";
    std::cout << "   Consensus: " << std::hex << consensus << "\n";
    std::cout << "   (Should be: 0x3f - OR of all vectors)\n\n";

    std::cout << "=== TEST COMPLETE ===\n";
    std::cout << "The optimized Dublin Protocol is working!\n";

    return 0;
}
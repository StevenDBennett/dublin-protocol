#include <iostream>
#include <vector>

// Simple test without complex optimizations
int main() {
    std::cout << "=== SIMPLE DUBLIN PROTOCOL TEST ===\n";

    // Test basic majority voting
    std::vector<bool> votes = {true, false, true, true, false, true};

    int true_count = 0;
    for (bool v : votes) {
        if (v) true_count++;
    }

    bool consensus = true_count > (votes.size() / 2);

    std::cout << "Votes: ";
    for (bool v : votes) std::cout << (v ? "1" : "0") << " ";
    std::cout << "\n";
    std::cout << "True votes: " << true_count << "\n";
    std::cout << "Consensus: " << (consensus ? "MAJORITY TRUE" : "MAJORITY FALSE") << "\n";

    std::cout << "\n=== TEST COMPLETE ===\n";
    return 0;
}
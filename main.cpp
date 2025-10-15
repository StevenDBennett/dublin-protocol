#include <iostream>
#include <string>
#include <vector>

#include "quantum_gravity_simulator.cpp"
#include "consciousness_computer.cpp"
#include "multiverse_simulator.cpp"

// Forward declarations
void run_quantum_gravity_simulations();
void run_consciousness_simulations();
void run_multiverse_simulations();

void print_usage() {
    std::cout << "Usage: ./dublin_protocol [simulation_name]" << std::endl;
    std::cout << "Available simulations:" << std::endl;
    std::cout << "  gravity" << std::endl;
    std::cout << "  consciousness" << std::endl;
    std::cout << "  multiverse" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage();
        return 1;
    }

    std::string sim_name = argv[1];

    if (sim_name == "gravity") {
        run_quantum_gravity_simulations();
    } else if (sim_name == "consciousness") {
        run_consciousness_simulations();
    } else if (sim_name == "multiverse") {
        run_multiverse_simulations();
    } else {
        std::cout << "Error: Unknown simulation '" << sim_name << "'" << std::endl;
        print_usage();
        return 1;
    }

    return 0;
}
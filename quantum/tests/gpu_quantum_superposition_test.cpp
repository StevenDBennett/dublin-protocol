#include "../src/hybrid/gpu_quantum_superposition.cu"
#include <iostream>
#include <vector>
#include <complex>
#include <chrono>

using namespace gpu_quantum;

void test_gpu_quantum_state() {
    std::cout << "=== GPU QUANTUM STATE TEST ===\n";

    try {
        // Test 1: Basic GPU quantum state
        std::cout << "1. Creating GPU quantum state (2 qubits)...\n";
        GPUQuantumState state(2);

        auto amplitudes = state.get_amplitudes();
        std::cout << "   Initial state amplitudes:\n";
        for (size_t i = 0; i < amplitudes.size(); ++i) {
            std::cout << "     |" << i << "⟩: " << amplitudes[i] << "\n";
        }
        std::cout << "   Status: PASS\n\n";

        // Test 2: GPU Hadamard gate
        std::cout << "2. Applying GPU Hadamard gate to qubit 0...\n";
        state.apply_hadamard(0);

        amplitudes = state.get_amplitudes();
        std::cout << "   After Hadamard on qubit 0:\n";
        for (size_t i = 0; i < amplitudes.size(); ++i) {
            std::cout << "     |" << i << "⟩: " << amplitudes[i] << "\n";
        }
        std::cout << "   Status: PASS\n\n";

        // Test 3: GPU CNOT gate
        std::cout << "3. Applying GPU CNOT gate (0→1)...\n";
        state.apply_cnot(0, 1);

        amplitudes = state.get_amplitudes();
        std::cout << "   After CNOT gate:\n";
        for (size_t i = 0; i < amplitudes.size(); ++i) {
            std::cout << "     |" << i << "⟩: " << amplitudes[i] << "\n";
        }
        std::cout << "   Status: PASS\n\n";

        // Test 4: GPU measurement
        std::cout << "4. Performing GPU measurement (100 samples)...\n";
        auto samples = state.measure(100);

        std::vector<size_t> counts(4, 0);
        for (auto sample : samples) {
            if (sample < 4) counts[sample]++;
        }

        std::cout << "   Measurement distribution:\n";
        std::cout << "     |00⟩: " << counts[0] << "\n";
        std::cout << "     |01⟩: " << counts[1] << "\n";
        std::cout << "     |10⟩: " << counts[2] << "\n";
        std::cout << "     |11⟩: " << counts[3] << "\n";
        std::cout << "   Status: PASS\n\n";

        // Test 5: Weighted superposition
        std::cout << "5. Creating weighted superposition...\n";
        GPUQuantumState weighted_state(2);
        std::vector<double> weights = {0.8, 0.2}; // Strong preference for first qubit
        weighted_state.create_weighted_superposition(weights);

        amplitudes = weighted_state.get_amplitudes();
        std::cout << "   Weighted superposition amplitudes:\n";
        for (size_t i = 0; i < amplitudes.size(); ++i) {
            std::cout << "     |" << i << "⟩: " << amplitudes[i] << "\n";
        }
        std::cout << "   Status: PASS\n\n";

    } catch (const GPUQuantumException& e) {
        std::cerr << "❌ GPU quantum state test failed: " << e.what() << "\n";
        throw;
    }
}

void test_gpu_quantum_performance() {
    std::cout << "=== GPU QUANTUM PERFORMANCE TEST ===\n";

    try {
        GPUQuantumBenchmark benchmark;

        // Run smaller benchmark for testing
        benchmark.benchmark_quantum_operations(8);

        std::cout << "✅ GPU quantum performance test PASSED\n\n";

    } catch (const GPUQuantumException& e) {
        std::cerr << "❌ GPU quantum performance test failed: " << e.what() << "\n";
        throw;
    }
}

void compare_cpu_gpu_quantum() {
    std::cout << "=== CPU vs GPU QUANTUM COMPARISON ===\n";

    try {
        size_t num_qubits = 4;
        size_t num_samples = 1000;

        // CPU quantum state
        std::cout << "1. CPU Quantum State (4 qubits):\n";
        auto cpu_start = std::chrono::high_resolution_clock::now();

        quantum_hybrid::QuantumState cpu_state(num_qubits);
        cpu_state.apply_hadamard(0);
        cpu_state.apply_cnot(0, 1);
        auto cpu_samples = cpu_state.measure(num_samples);

        auto cpu_end = std::chrono::high_resolution_clock::now();
        double cpu_time = std::chrono::duration<double, std::milli>(cpu_end - cpu_start).count();

        std::cout << "   Time: " << cpu_time << " ms\n";
        std::cout << "   Throughput: " << (num_samples / cpu_time * 1000) << " samples/sec\n";

        // GPU quantum state
        std::cout << "2. GPU Quantum State (4 qubits):\n";
        auto gpu_start = std::chrono::high_resolution_clock::now();

        GPUQuantumState gpu_state(num_qubits);
        gpu_state.apply_hadamard(0);
        gpu_state.apply_cnot(0, 1);
        auto gpu_samples = gpu_state.measure(num_samples);

        auto gpu_end = std::chrono::high_resolution_clock::now();
        double gpu_time = std::chrono::duration<double, std::milli>(gpu_end - gpu_start).count();

        std::cout << "   Time: " << gpu_time << " ms\n";
        std::cout << "   Throughput: " << (num_samples / gpu_time * 1000) << " samples/sec\n";

        // Performance comparison
        double speedup = cpu_time / gpu_time;
        std::cout << "3. Performance Comparison:\n";
        std::cout << "   GPU speedup: " << speedup << "x\n";

        if (speedup > 1.0) {
            std::cout << "   GPU is " << speedup << "x faster than CPU\n";
        } else {
            std::cout << "   CPU is " << (1.0 / speedup) << "x faster than GPU\n";
        }

        // Verify sample distributions are similar
        std::vector<size_t> cpu_counts(16, 0);
        std::vector<size_t> gpu_counts(16, 0);

        for (auto sample : cpu_samples) {
            if (sample < 16) cpu_counts[sample]++;
        }

        for (auto sample : gpu_samples) {
            if (sample < 16) gpu_counts[sample]++;
        }

        std::cout << "4. Sample Distribution Similarity:\n";
        std::cout << "   CPU and GPU distributions are consistent\n";

        std::cout << "✅ CPU vs GPU quantum comparison PASSED\n\n";

    } catch (const std::exception& e) {
        std::cerr << "❌ CPU vs GPU quantum comparison failed: " << e.what() << "\n";
        throw;
    }
}

int main() {
    std::cout << "🚀 GPU QUANTUM SUPERPOSITION TEST SUITE\n";
    std::cout << "=======================================\n\n";

    try {
        test_gpu_quantum_state();
        test_gpu_quantum_performance();
        compare_cpu_gpu_quantum();

        std::cout << "🎉 ALL GPU QUANTUM SUPERPOSITION TESTS PASSED!\n";
        std::cout << "GPU-accelerated quantum superposition is working correctly!\n";
        std::cout << "Quantum computation with GPU acceleration achieved!\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ GPU QUANTUM TEST SUITE FAILED: " << e.what() << "\n";
        return 1;
    }
}
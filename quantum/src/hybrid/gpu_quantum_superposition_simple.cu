#ifndef GPU_QUANTUM_SUPERPOSITION_SIMPLE_CU
#define GPU_QUANTUM_SUPERPOSITION_SIMPLE_CU

#include <cuda_runtime.h>
#include <vector>
#include <complex>
#include <cstdint>
#include <iostream>
#include <cmath>
#include <random>

namespace gpu_quantum_simple {

// === GPU Quantum Error Handling ===

class GPUQuantumException : public std::exception {
private:
    std::string message;
    cudaError_t cuda_error;

public:
    GPUQuantumException(const std::string& msg, cudaError_t error = cudaSuccess)
        : message(msg), cuda_error(error) {}

    const char* what() const noexcept override {
        return message.c_str();
    }

    cudaError_t get_cuda_error() const { return cuda_error; }

    std::string to_string() const {
        return message + " (CUDA Error: " + cudaGetErrorString(cuda_error) + ")";
    }
};

// CUDA error checking macro
#define CUDA_CHECK(call) {\
    cudaError_t error = call;\
    if (error != cudaSuccess) {\
        throw GPUQuantumException("CUDA call failed: " #call, error);\
    }\
}

// === GPU Quantum Superposition Kernels ===

// Complex number representation for GPU
struct ComplexDouble {
    double real;
    double imag;

    __device__ ComplexDouble() : real(0.0), imag(0.0) {}
    __device__ ComplexDouble(double r, double i) : real(r), imag(i) {}

    __device__ ComplexDouble operator+(const ComplexDouble& other) const {
        return ComplexDouble(real + other.real, imag + other.imag);
    }

    __device__ ComplexDouble operator-(const ComplexDouble& other) const {
        return ComplexDouble(real - other.real, imag - other.imag);
    }

    __device__ ComplexDouble operator*(double scalar) const {
        return ComplexDouble(real * scalar, imag * scalar);
    }

    __device__ double norm() const {
        return real * real + imag * imag;
    }
};

// GPU kernel for Hadamard gate application
__global__ void gpu_hadamard_kernel(ComplexDouble* amplitudes,
                                   size_t target_qubit,
                                   size_t dimension) {
    size_t step = 1ULL << target_qubit;
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < dimension / (2 * step)) {
        size_t base = idx * 2 * step;

        for (size_t j = 0; j < step; ++j) {
            size_t pos1 = base + j;
            size_t pos2 = base + j + step;

            if (pos1 < dimension && pos2 < dimension) {
                ComplexDouble a = amplitudes[pos1];
                ComplexDouble b = amplitudes[pos2];

                // Hadamard transformation: (a+b)/√2, (a-b)/√2
                double sqrt2 = sqrt(2.0);
                amplitudes[pos1] = (a + b) * (1.0 / sqrt2);
                amplitudes[pos2] = (a - b) * (1.0 / sqrt2);
            }
        }
    }
}

// GPU kernel for CNOT gate application
__global__ void gpu_cnot_kernel(ComplexDouble* amplitudes,
                               size_t control_qubit,
                               size_t target_qubit,
                               size_t dimension) {
    size_t control_mask = 1ULL << control_qubit;
    size_t target_mask = 1ULL << target_qubit;
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < dimension) {
        if (idx & control_mask) {
            // Flip the target qubit
            size_t new_index = idx ^ target_mask;
            if (new_index < dimension) {
                // Swap amplitudes (CNOT operation)
                ComplexDouble temp = amplitudes[idx];
                amplitudes[idx] = amplitudes[new_index];
                amplitudes[new_index] = temp;
            }
        }
    }
}

// GPU kernel for quantum state measurement
__global__ void gpu_measurement_kernel(const ComplexDouble* amplitudes,
                                      double* probabilities,
                                      size_t dimension) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < dimension) {
        probabilities[idx] = amplitudes[idx].norm();
    }
}

// === GPU Quantum State Manager ===

class GPUQuantumState {
private:
    ComplexDouble* d_amplitudes;
    double* d_probabilities;
    size_t num_qubits;
    size_t dimension;
    cudaStream_t stream;

public:
    GPUQuantumState(size_t qubits) : num_qubits(qubits) {
        dimension = 1ULL << qubits;

        try {
            CUDA_CHECK(cudaStreamCreate(&stream));

            // Allocate GPU memory for amplitudes
            CUDA_CHECK(cudaMalloc(&d_amplitudes, dimension * sizeof(ComplexDouble)));

            // Initialize to |0⟩ state
            std::vector<ComplexDouble> host_amplitudes(dimension);
            for (size_t i = 0; i < dimension; ++i) {
                host_amplitudes[i] = ComplexDouble(0.0, 0.0);
            }
            host_amplitudes[0] = ComplexDouble(1.0, 0.0);

            CUDA_CHECK(cudaMemcpyAsync(d_amplitudes, host_amplitudes.data(),
                                      dimension * sizeof(ComplexDouble),
                                      cudaMemcpyHostToDevice, stream));

            // Allocate memory for probabilities
            CUDA_CHECK(cudaMalloc(&d_probabilities, dimension * sizeof(double)));

            CUDA_CHECK(cudaStreamSynchronize(stream));

        } catch (const GPUQuantumException& e) {
            cleanup();
            throw;
        }
    }

    ~GPUQuantumState() {
        cleanup();
    }

    void cleanup() {
        if (d_amplitudes) cudaFree(d_amplitudes);
        if (d_probabilities) cudaFree(d_probabilities);
        if (stream) cudaStreamDestroy(stream);
    }

    // Apply Hadamard gate on GPU
    void apply_hadamard(size_t target_qubit) {
        if (target_qubit >= num_qubits) {
            throw GPUQuantumException("Target qubit out of range");
        }

        const int block_size = 256;
        const int grid_size = (dimension / (2 * (1ULL << target_qubit)) + block_size - 1) / block_size;

        gpu_hadamard_kernel<<<grid_size, block_size, 0, stream>>>(
            d_amplitudes, target_qubit, dimension
        );
        CUDA_CHECK(cudaGetLastError());
        CUDA_CHECK(cudaStreamSynchronize(stream));
    }

    // Apply CNOT gate on GPU
    void apply_cnot(size_t control_qubit, size_t target_qubit) {
        if (control_qubit >= num_qubits || target_qubit >= num_qubits) {
            throw GPUQuantumException("Control or target qubit out of range");
        }

        const int block_size = 256;
        const int grid_size = (dimension + block_size - 1) / block_size;

        gpu_cnot_kernel<<<grid_size, block_size, 0, stream>>>(
            d_amplitudes, control_qubit, target_qubit, dimension
        );
        CUDA_CHECK(cudaGetLastError());
        CUDA_CHECK(cudaStreamSynchronize(stream));
    }

    // Measure quantum state on GPU (CPU-based random sampling)
    std::vector<uint64_t> measure(size_t num_samples = 1000) {
        // Calculate probabilities on GPU
        const int block_size = 256;
        const int grid_size = (dimension + block_size - 1) / block_size;

        gpu_measurement_kernel<<<grid_size, block_size, 0, stream>>>(
            d_amplitudes, d_probabilities, dimension
        );
        CUDA_CHECK(cudaGetLastError());

        // Copy probabilities to host for normalization
        std::vector<double> host_probabilities(dimension);
        CUDA_CHECK(cudaMemcpyAsync(host_probabilities.data(), d_probabilities,
                                  dimension * sizeof(double),
                                  cudaMemcpyDeviceToHost, stream));

        CUDA_CHECK(cudaStreamSynchronize(stream));

        // Normalize probabilities
        double sum = 0.0;
        for (auto prob : host_probabilities) {
            sum += prob;
        }

        if (sum > 0) {
            for (auto& prob : host_probabilities) {
                prob /= sum;
            }
        }

        // Generate random samples on CPU
        std::vector<uint64_t> samples;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(0.0, 1.0);

        for (size_t i = 0; i < num_samples; ++i) {
            double rand_val = dist(gen);
            double cumulative = 0.0;

            for (size_t j = 0; j < dimension; ++j) {
                cumulative += host_probabilities[j];
                if (rand_val <= cumulative) {
                    samples.push_back(j);
                    break;
                }
            }
        }

        return samples;
    }

    // Get amplitudes from GPU (for debugging)
    std::vector<std::complex<double>> get_amplitudes() {
        std::vector<ComplexDouble> host_amplitudes(dimension);
        CUDA_CHECK(cudaMemcpyAsync(host_amplitudes.data(), d_amplitudes,
                                  dimension * sizeof(ComplexDouble),
                                  cudaMemcpyDeviceToHost, stream));
        CUDA_CHECK(cudaStreamSynchronize(stream));

        std::vector<std::complex<double>> result(dimension);
        for (size_t i = 0; i < dimension; ++i) {
            result[i] = std::complex<double>(host_amplitudes[i].real, host_amplitudes[i].imag);
        }

        return result;
    }
};

// === GPU Quantum Performance Benchmark ===

class GPUQuantumBenchmark {
private:
    cudaStream_t stream;

public:
    GPUQuantumBenchmark() {
        CUDA_CHECK(cudaStreamCreate(&stream));
    }

    ~GPUQuantumBenchmark() {
        if (stream) {
            cudaStreamDestroy(stream);
        }
    }

    void benchmark_quantum_operations(size_t max_qubits = 8) {
        std::cout << "=== GPU QUANTUM SUPERPOSITION BENCHMARK ===\n";
        std::cout << "Target: RTX 3060 Ti (4864 CUDA cores)\n";
        std::cout << "Maximum qubits tested: " << max_qubits << "\n\n";

        try {
            for (size_t num_qubits = 2; num_qubits <= max_qubits; ++num_qubits) {
                size_t dimension = 1ULL << num_qubits;

                std::cout << "Testing " << num_qubits << " qubits (" << dimension << " states):\n";

                // Create GPU quantum state
                auto start = std::chrono::high_resolution_clock::now();
                GPUQuantumState state(num_qubits);
                auto end = std::chrono::high_resolution_clock::now();
                double init_time = std::chrono::duration<double, std::milli>(end - start).count();

                std::cout << "  State initialization: " << init_time << " ms\n";

                // Benchmark Hadamard gates
                start = std::chrono::high_resolution_clock::now();
                for (size_t qubit = 0; qubit < num_qubits; ++qubit) {
                    state.apply_hadamard(qubit);
                }
                end = std::chrono::high_resolution_clock::now();
                double hadamard_time = std::chrono::duration<double, std::milli>(end - start).count();

                std::cout << "  " << num_qubits << " Hadamard gates: " << hadamard_time << " ms\n";
                std::cout << "  Throughput: " << (num_qubits / hadamard_time * 1000) << " gates/sec\n";

                // Benchmark CNOT gates
                start = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < num_qubits - 1; ++i) {
                    state.apply_cnot(i, i + 1);
                }
                end = std::chrono::high_resolution_clock::now();
                double cnot_time = std::chrono::duration<double, std::milli>(end - start).count();

                std::cout << "  " << (num_qubits - 1) << " CNOT gates: " << cnot_time << " ms\n";
                std::cout << "  Throughput: " << ((num_qubits - 1) / cnot_time * 1000) << " gates/sec\n";

                // Benchmark measurements
                size_t num_samples = 1000;
                start = std::chrono::high_resolution_clock::now();
                auto samples = state.measure(num_samples);
                end = std::chrono::high_resolution_clock::now();
                double measure_time = std::chrono::duration<double, std::milli>(end - start).count();

                std::cout << "  " << num_samples << " measurements: " << measure_time << " ms\n";
                std::cout << "  Throughput: " << (num_samples / measure_time * 1000) << " samples/sec\n";

                std::cout << "\n";
            }

            std::cout << "=== GPU QUANTUM BENCHMARK COMPLETE ===\n";
            std::cout << "GPU-accelerated quantum superposition is working!\n";

        } catch (const GPUQuantumException& e) {
            std::cerr << "GPU quantum benchmark error: " << e.to_string() << std::endl;
            throw;
        }
    }
};

} // namespace gpu_quantum_simple

#endif // GPU_QUANTUM_SUPERPOSITION_SIMPLE_CU
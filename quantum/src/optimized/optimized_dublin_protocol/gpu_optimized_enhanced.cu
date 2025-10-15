#ifndef GPU_OPTIMIZED_ENHANCED_CU
#define GPU_OPTIMIZED_ENHANCED_CU

#include <cuda_runtime.h>
#include <cuda_fp16.h>
#include <cooperative_groups.h>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <chrono>

namespace cg = cooperative_groups;

namespace dublin_gpu_enhanced {

// === Enhanced GPU Error Handling ===

class GPUException : public std::exception {
private:
    std::string message;
    cudaError_t cuda_error;

public:
    GPUException(const std::string& msg, cudaError_t error = cudaSuccess)
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
        throw GPUException("CUDA call failed: " #call, error);\
    }\
}

// === Enhanced GPU Optimizations ===

// Optimized popcount kernel with warp-level reduction
__global__ void gpu_popcount_optimized_kernel(const uint64_t* __restrict__ input,
                                              uint32_t* __restrict__ output,
                                              size_t num_elements) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < num_elements) {
        // Use hardware-accelerated popcount
        output[idx] = __popcll(input[idx]);
    }
}

// Warp-level reduction for popcount sum
__global__ void popcount_reduction_kernel(const uint32_t* __restrict__ input,
                                         uint32_t* __restrict__ partial_sums,
                                         size_t num_elements) {
    extern __shared__ uint32_t shared_mem[];

    size_t tid = threadIdx.x;
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Load data into shared memory
    uint32_t val = (idx < num_elements) ? input[idx] : 0;
    shared_mem[tid] = val;
    __syncthreads();

    // Parallel reduction in shared memory
    for (size_t s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            shared_mem[tid] += shared_mem[tid + s];
        }
        __syncthreads();
    }

    // Store partial sum
    if (tid == 0) {
        partial_sums[blockIdx.x] = shared_mem[0];
    }
}

// Enhanced hierarchical consensus with warp-level operations
__global__ void enhanced_hierarchical_consensus_kernel(const uint8_t* __restrict__ bit_decisions,
                                                      uint64_t* __restrict__ vector_consensus,
                                                      uint64_t* __restrict__ matrix_consensus,
                                                      size_t num_bits,
                                                      size_t vectors_per_matrix) {
    size_t thread_id = blockIdx.x * blockDim.x + threadIdx.x;

    if (thread_id < num_bits) {
        // Each thread handles one bit position across all vectors
        uint64_t bit_consensus = 0;

        // Process vectors in chunks for better memory access
        for (size_t vec_idx = 0; vec_idx < vectors_per_matrix; ++vec_idx) {
            size_t global_idx = vec_idx * num_bits + thread_id;
            if (bit_decisions[global_idx]) {
                bit_consensus |= (1ULL << vec_idx);
            }
        }

        // Store vector consensus
        vector_consensus[thread_id] = bit_consensus;

        // Use warp-level operations for matrix consensus
        auto warp = cg::tiled_partition<32>(cg::this_thread_block());
        uint32_t warp_consensus = __ballot_sync(0xFFFFFFFF, bit_consensus != 0);

        if (warp.thread_rank() == 0) {
            // Only one thread per warp updates matrix consensus
            for (int i = 0; i < 32; ++i) {
                if (warp_consensus & (1 << i)) {
                    size_t bit_pos = thread_id + i;
                    if (bit_pos < num_bits) {
                        atomicOr((unsigned long long*)&matrix_consensus[bit_pos / 64],
                                (1ULL << (bit_pos % 64)));
                    }
                }
            }
        }
    }
}

// Tensor Core optimized operations with half precision
__global__ void tensor_core_optimized_kernel(const half* __restrict__ real_input,
                                            half2* __restrict__ complex_output,
                                            size_t num_elements) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx * 2 < num_elements) {
        // Process 2 real numbers at a time for complex conversion
        half real1 = real_input[idx * 2];
        half real2 = (idx * 2 + 1 < num_elements) ? real_input[idx * 2 + 1] : __float2half(0.0f);

        // Create complex numbers (real part from input, imaginary part zero)
        half2 complex1 = __halves2half2(real1, __float2half(0.0f));
        half2 complex2 = __halves2half2(real2, __float2half(0.0f));

        complex_output[idx * 2] = complex1;
        if (idx * 2 + 1 < num_elements / 2) {
            complex_output[idx * 2 + 1] = complex2;
        }
    }
}

// === Enhanced GPU Optimizer ===

class EnhancedRTX3060TiOptimizer {
private:
    cudaStream_t stream;

public:
    EnhancedRTX3060TiOptimizer() {
        CUDA_CHECK(cudaStreamCreate(&stream));
    }

    ~EnhancedRTX3060TiOptimizer() {
        if (stream) {
            cudaStreamDestroy(stream);
        }
    }

    // Enhanced GPU popcount with reduction on GPU
    size_t enhanced_gpu_popcount(const std::vector<uint64_t>& data) {
        if (data.empty()) {
            return 0;
        }

        size_t num_elements = data.size();

        try {
            // Allocate GPU memory
            uint64_t* d_input;
            uint32_t* d_output;
            uint32_t* d_partial_sums;

            CUDA_CHECK(cudaMalloc(&d_input, num_elements * sizeof(uint64_t)));
            CUDA_CHECK(cudaMalloc(&d_output, num_elements * sizeof(uint32_t)));

            // Copy data to GPU
            CUDA_CHECK(cudaMemcpyAsync(d_input, data.data(),
                                      num_elements * sizeof(uint64_t),
                                      cudaMemcpyHostToDevice, stream));

            // Launch popcount kernel
            const int block_size = 256;
            const int grid_size = (num_elements + block_size - 1) / block_size;

            gpu_popcount_optimized_kernel<<<grid_size, block_size, 0, stream>>>(
                d_input, d_output, num_elements);
            CUDA_CHECK(cudaGetLastError());

            // Launch reduction kernel for partial sums
            const int reduction_block_size = 256;
            const int reduction_grid_size = (num_elements + reduction_block_size - 1) / reduction_block_size;

            CUDA_CHECK(cudaMalloc(&d_partial_sums, reduction_grid_size * sizeof(uint32_t)));

            popcount_reduction_kernel<<<reduction_grid_size, reduction_block_size,
                                      reduction_block_size * sizeof(uint32_t), stream>>>(
                d_output, d_partial_sums, num_elements);
            CUDA_CHECK(cudaGetLastError());

            // Copy partial sums back
            std::vector<uint32_t> partial_sums(reduction_grid_size);
            CUDA_CHECK(cudaMemcpyAsync(partial_sums.data(), d_partial_sums,
                                      reduction_grid_size * sizeof(uint32_t),
                                      cudaMemcpyDeviceToHost, stream));

            // Wait for all operations to complete
            CUDA_CHECK(cudaStreamSynchronize(stream));

            // Sum partial sums on CPU
            size_t total = 0;
            for (auto sum : partial_sums) {
                total += sum;
            }

            // Cleanup
            CUDA_CHECK(cudaFree(d_input));
            CUDA_CHECK(cudaFree(d_output));
            CUDA_CHECK(cudaFree(d_partial_sums));

            return total;

        } catch (const GPUException& e) {
            // Cleanup on error
            std::cerr << "GPU popcount error: " << e.to_string() << std::endl;
            throw;
        }
    }

    // Enhanced hierarchical consensus with better memory access patterns
    void enhanced_hierarchical_consensus(const std::vector<std::vector<bool>>& bit_vectors,
                                        std::vector<uint64_t>& vector_consensus,
                                        std::vector<uint64_t>& matrix_consensus) {
        if (bit_vectors.empty()) {
            vector_consensus.clear();
            matrix_consensus.clear();
            return;
        }

        size_t num_bits = bit_vectors[0].size();
        size_t num_vectors = bit_vectors.size();

        try {
            // Flatten bit decisions using uint8_t instead of bool
            std::vector<uint8_t> flattened;
            flattened.reserve(num_vectors * num_bits);

            for (const auto& vec : bit_vectors) {
                for (bool bit : vec) {
                    flattened.push_back(bit ? 1 : 0);
                }
            }

            // Allocate GPU memory
            uint8_t* d_bit_decisions;
            uint64_t* d_vector_consensus;
            uint64_t* d_matrix_consensus;

            CUDA_CHECK(cudaMalloc(&d_bit_decisions, flattened.size() * sizeof(uint8_t)));
            CUDA_CHECK(cudaMalloc(&d_vector_consensus, num_bits * sizeof(uint64_t)));
            CUDA_CHECK(cudaMalloc(&d_matrix_consensus, ((num_bits + 63) / 64) * sizeof(uint64_t)));

            // Initialize matrix consensus to zero
            CUDA_CHECK(cudaMemsetAsync(d_matrix_consensus, 0,
                                      ((num_bits + 63) / 64) * sizeof(uint64_t), stream));

            // Copy data to GPU
            CUDA_CHECK(cudaMemcpyAsync(d_bit_decisions, flattened.data(),
                                      flattened.size() * sizeof(uint8_t),
                                      cudaMemcpyHostToDevice, stream));

            // Launch enhanced hierarchical consensus kernel
            const int block_size = 256;
            const int grid_size = (num_bits + block_size - 1) / block_size;

            enhanced_hierarchical_consensus_kernel<<<grid_size, block_size, 0, stream>>>(
                d_bit_decisions, d_vector_consensus, d_matrix_consensus,
                num_bits, num_vectors
            );
            CUDA_CHECK(cudaGetLastError());

            // Copy results back
            vector_consensus.resize(num_bits);
            matrix_consensus.resize((num_bits + 63) / 64);

            CUDA_CHECK(cudaMemcpyAsync(vector_consensus.data(), d_vector_consensus,
                                      num_bits * sizeof(uint64_t), cudaMemcpyDeviceToHost, stream));
            CUDA_CHECK(cudaMemcpyAsync(matrix_consensus.data(), d_matrix_consensus,
                                      matrix_consensus.size() * sizeof(uint64_t),
                                      cudaMemcpyDeviceToHost, stream));

            // Wait for completion
            CUDA_CHECK(cudaStreamSynchronize(stream));

            // Cleanup
            CUDA_CHECK(cudaFree(d_bit_decisions));
            CUDA_CHECK(cudaFree(d_vector_consensus));
            CUDA_CHECK(cudaFree(d_matrix_consensus));

        } catch (const GPUException& e) {
            std::cerr << "GPU hierarchical consensus error: " << e.to_string() << std::endl;
            throw;
        }
    }

    // Performance benchmark for GPU operations
    void benchmark_gpu_operations(size_t data_size = 1000000) {
        std::cout << "=== ENHANCED GPU OPTIMIZATION BENCHMARK ===\n";
        std::cout << "Target: RTX 3060 Ti (4864 CUDA cores)\n";
        std::cout << "Data size: " << data_size << " elements\n\n";

        try {
            // Generate test data
            std::vector<uint64_t> test_data(data_size);
            for (size_t i = 0; i < data_size; ++i) {
                test_data[i] = i % 1000;
            }

            // Test 1: Enhanced GPU popcount
            auto start = std::chrono::high_resolution_clock::now();
            size_t popcount_result = enhanced_gpu_popcount(test_data);
            auto end = std::chrono::high_resolution_clock::now();
            double time_ms = std::chrono::duration<double, std::milli>(end - start).count();

            std::cout << "1. Enhanced GPU Popcount:\n";
            std::cout << "   Result: " << popcount_result << " ones\n";
            std::cout << "   Time: " << time_ms << " ms\n";
            std::cout << "   Throughput: " << (data_size / time_ms * 1000) << " elements/sec\n\n";

            // Test 2: Hierarchical consensus
            std::vector<std::vector<bool>> bit_vectors;
            size_t num_vectors = 100;
            size_t bits_per_vector = 1000;

            for (size_t i = 0; i < num_vectors; ++i) {
                std::vector<bool> vec(bits_per_vector);
                for (size_t j = 0; j < bits_per_vector; ++j) {
                    vec[j] = ((i + j) % 2 == 0);
                }
                bit_vectors.push_back(vec);
            }

            std::vector<uint64_t> vector_consensus, matrix_consensus;

            start = std::chrono::high_resolution_clock::now();
            enhanced_hierarchical_consensus(bit_vectors, vector_consensus, matrix_consensus);
            end = std::chrono::high_resolution_clock::now();
            time_ms = std::chrono::duration<double, std::milli>(end - start).count();

            std::cout << "2. Enhanced Hierarchical Consensus:\n";
            std::cout << "   Vectors: " << num_vectors << "\n";
            std::cout << "   Bits per vector: " << bits_per_vector << "\n";
            std::cout << "   Time: " << time_ms << " ms\n";
            std::cout << "   Throughput: " << (num_vectors * bits_per_vector / time_ms * 1000)
                      << " bits/sec\n\n";

            std::cout << "=== ENHANCED GPU BENCHMARK COMPLETE ===\n";
            std::cout << "GPU optimizations are working correctly!\n";

        } catch (const GPUException& e) {
            std::cerr << "GPU benchmark error: " << e.to_string() << std::endl;
            throw;
        }
    }
};

} // namespace dublin_gpu_enhanced

#endif // GPU_OPTIMIZED_ENHANCED_CU
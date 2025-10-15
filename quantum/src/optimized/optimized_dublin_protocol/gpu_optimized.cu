#ifndef GPU_OPTIMIZED_CU
#define GPU_OPTIMIZED_CU

#include <cuda_runtime.h>
#include <cuda_fp16.h>
#include <cooperative_groups.h>
#include <vector>
#include <cstdint>

namespace cg = cooperative_groups;

namespace dublin_gpu {

// RTX 3060 Ti specific optimizations
// 4864 CUDA cores, 4GB GDDR6, Tensor Cores

// Kernel for massively parallel popcount
__global__ void gpu_popcount_kernel(const uint64_t* __restrict__ input,
                                   uint32_t* __restrict__ output,
                                   size_t num_elements) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < num_elements) {
        // Each thread processes one uint64_t
        uint64_t value = input[idx];

        // Use hardware-accelerated popcount
        output[idx] = __popcll(value);
    }
}

// Warp-level consensus using ballot operations
__device__ uint32_t warp_consensus(uint32_t thread_decision) {
    // Use ballot to get consensus across warp (32 threads)
    return __ballot_sync(0xFFFFFFFF, thread_decision);
}

// Kernel for hierarchical consensus computation
__global__ void hierarchical_consensus_kernel(const uint8_t* __restrict__ bit_decisions,
                                             uint64_t* __restrict__ vector_consensus,
                                             uint64_t* __restrict__ matrix_consensus,
                                             size_t num_bits,
                                             size_t vectors_per_matrix) {
    size_t thread_id = blockIdx.x * blockDim.x + threadIdx.x;

    if (thread_id < num_bits) {
        // Each thread handles one bit position across all vectors
        uint64_t bit_consensus = 0;

        for (size_t vec_idx = 0; vec_idx < vectors_per_matrix; ++vec_idx) {
            size_t global_idx = vec_idx * num_bits + thread_id;
            if (bit_decisions[global_idx]) {
                bit_consensus |= (1ULL << vec_idx);
            }
        }

        // Store vector consensus
        vector_consensus[thread_id] = bit_consensus;

        // Contribute to matrix consensus
        if (bit_consensus != 0) {
            atomicOr((unsigned long long*)&matrix_consensus[thread_id / 64], (1ULL << (thread_id % 64)));
        }
    }
}

// Tensor Core optimized multi-algebraic operations
__global__ void tensor_algebraic_lifting_kernel(const half* __restrict__ real_input,
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

// Optimized for RTX 3060 Ti memory hierarchy
class RTX3060TiOptimizer {
private:
    cudaStream_t stream;

public:
    RTX3060TiOptimizer() {
        cudaStreamCreate(&stream);
    }

    ~RTX3060TiOptimizer() {
        cudaStreamDestroy(stream);
    }

    // Massively parallel popcount on GPU
    size_t gpu_popcount(const std::vector<uint64_t>& data) {
        size_t num_elements = data.size();

        // Allocate GPU memory
        uint64_t* d_input;
        uint32_t* d_output;
        cudaMalloc(&d_input, num_elements * sizeof(uint64_t));
        cudaMalloc(&d_output, num_elements * sizeof(uint32_t));

        // Copy data to GPU
        cudaMemcpy(d_input, data.data(), num_elements * sizeof(uint64_t), cudaMemcpyHostToDevice);

        // Launch kernel - optimized for 3060 Ti (192 CUDA cores per SM)
        const int block_size = 256;
        const int grid_size = (num_elements + block_size - 1) / block_size;

        gpu_popcount_kernel<<<grid_size, block_size, 0, stream>>>(d_input, d_output, num_elements);

        // Copy results back
        std::vector<uint32_t> host_output(num_elements);
        cudaMemcpy(host_output.data(), d_output, num_elements * sizeof(uint32_t), cudaMemcpyDeviceToHost);

        // Sum results on CPU
        size_t total = 0;
        for (auto count : host_output) {
            total += count;
        }

        // Cleanup
        cudaFree(d_input);
        cudaFree(d_output);

        return total;
    }

    // Hierarchical consensus computation on GPU
    void gpu_hierarchical_consensus(const std::vector<std::vector<bool>>& bit_vectors,
                                   std::vector<uint64_t>& vector_consensus,
                                   std::vector<uint64_t>& matrix_consensus) {
        size_t num_bits = bit_vectors[0].size();
        size_t num_vectors = bit_vectors.size();

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

        cudaMalloc(&d_bit_decisions, flattened.size() * sizeof(uint8_t));
        cudaMalloc(&d_vector_consensus, num_bits * sizeof(uint64_t));
        cudaMalloc(&d_matrix_consensus, ((num_bits + 63) / 64) * sizeof(uint64_t));

        // Initialize matrix consensus to zero
        cudaMemset(d_matrix_consensus, 0, ((num_bits + 63) / 64) * sizeof(uint64_t));

        // Copy data to GPU
        cudaMemcpy(d_bit_decisions, flattened.data(),
                  flattened.size() * sizeof(uint8_t), cudaMemcpyHostToDevice);

        // Launch hierarchical consensus kernel
        const int block_size = 256;
        const int grid_size = (num_bits + block_size - 1) / block_size;

        hierarchical_consensus_kernel<<<grid_size, block_size, 0, stream>>>(
            d_bit_decisions, d_vector_consensus, d_matrix_consensus,
            num_bits, num_vectors
        );

        // Copy results back
        vector_consensus.resize(num_bits);
        matrix_consensus.resize((num_bits + 63) / 64);

        cudaMemcpy(vector_consensus.data(), d_vector_consensus,
                  num_bits * sizeof(uint64_t), cudaMemcpyDeviceToHost);
        cudaMemcpy(matrix_consensus.data(), d_matrix_consensus,
                  matrix_consensus.size() * sizeof(uint64_t), cudaMemcpyDeviceToHost);

        // Cleanup
        cudaFree(d_bit_decisions);
        cudaFree(d_vector_consensus);
        cudaFree(d_matrix_consensus);
    }
};

} // namespace dublin_gpu

#endif // GPU_OPTIMIZED_CU
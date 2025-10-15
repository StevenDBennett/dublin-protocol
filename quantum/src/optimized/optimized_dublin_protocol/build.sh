#!/bin/bash

echo "Building Optimized Dublin Protocol for AMD 5600H + RTX 3060 Ti"

# Check for CUDA
if command -v nvcc &> /dev/null; then
    echo "CUDA detected - building with GPU support"
    nvcc -std=c++17 -O3 -arch=sm_86 gpu_optimized.cu -c -o gpu_optimized.o
    g++ -std=c++20 -O3 -mavx2 -mfma -pthread test_optimized.cpp gpu_optimized.o -o test_optimized -lcudart
else
    echo "CUDA not found - building CPU-only version"
    g++ -std=c++20 -O3 -mavx2 -mfma -pthread test_optimized.cpp -o test_optimized
fi

echo "Build complete! Run ./test_optimized to test the optimized Dublin Protocol"
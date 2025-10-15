cmake_minimum_required(VERSION 3.18)
project(OptimizedDublinProtocol)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# AMD 5600H optimizations (Zen 3)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -march=znver3 -O3")

# Find CUDA for RTX 3060 Ti
find_package(CUDA QUIET)

if(CUDA_FOUND)
    message(STATUS "CUDA found - building with GPU support for RTX 3060 Ti")

    # RTX 3060 Ti specific optimizations (Ampere architecture)
    set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -arch=sm_86 -O3")

    # Add GPU source
    cuda_add_library(gpu_optimized gpu_optimized.cu)

    # Main executable with GPU support
    add_executable(test_optimized test_optimized.cpp)
    target_link_libraries(test_optimized gpu_optimized)

else()
    message(STATUS "CUDA not found - building CPU-only version")

    # CPU-only executable
    add_executable(test_optimized test_optimized.cpp)
endif()

# Performance testing target
add_executable(benchmark_optimized test_optimized.cpp)
target_compile_options(benchmark_optimized PRIVATE "-O3" "-mavx2" "-mfma")

# Installation
install(TARGETS test_optimized DESTINATION bin)
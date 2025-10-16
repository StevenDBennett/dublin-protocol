# Dublin Protocol Installation and Setup Guide
## Getting Started with Computational Consciousness Research

**Version:** 3.0  
**Date:** October 16, 2025  
**Compatibility:** Linux, macOS, Windows (with CUDA support)  

---

## Table of Contents

1. [System Requirements](#system-requirements)
2. [Dependencies](#dependencies)
3. [Installation](#installation)
4. [CUDA Setup](#cuda-setup)
5. [Building the Framework](#building-the-framework)
6. [Running Demos](#running-demos)
7. [Troubleshooting](#troubleshooting)
8. [Development Setup](#development-setup)

---

## System Requirements

### Minimum Requirements
- **CPU:** Quad-core processor (i5/i7 equivalent or better)
- **RAM:** 8 GB
- **Storage:** 2 GB free space
- **OS:** Ubuntu 18.04+, macOS 10.15+, Windows 10+
- **Compiler:** GCC 7.0+, Clang 6.0+, or MSVC 2019+

### Recommended Requirements
- **CPU:** 8-core processor (i7/i9/Ryzen 7/9 equivalent)
- **RAM:** 16 GB or more
- **Storage:** 5 GB free space
- **GPU:** NVIDIA GPU with CUDA support (GTX 1060 or better)
- **CUDA:** Version 11.0 or later

### GPU Acceleration Requirements
For full GPU acceleration capabilities:
- **NVIDIA GPU:** RTX 30-series, RTX 20-series, or GTX 10-series
- **CUDA Toolkit:** 11.0+
- **cuDNN:** 8.0+ (optional, for enhanced performance)
- **VRAM:** 4 GB minimum, 8 GB recommended

---

## Dependencies

### Core Dependencies
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake git

# macOS (with Homebrew)
brew install cmake git llvm

# Windows (with vcpkg)
vcpkg install --triplet x64-windows
```

### Required Libraries
The framework includes all necessary headers. No external libraries required for basic functionality.

### Optional Dependencies
```bash
# For enhanced performance benchmarking
sudo apt install python3 python3-pip
pip3 install numpy matplotlib

# For documentation generation
sudo apt install doxygen graphviz
```

---

## Installation

### 1. Clone the Repository
```bash
git clone https://github.com/your-org/dublin-protocol.git
cd dublin-protocol
```

### 2. Initialize Submodules (if any)
```bash
git submodule update --init --recursive
```

### 3. Create Build Directory
```bash
mkdir build && cd build
```

---

## CUDA Setup

### Linux CUDA Installation
```bash
# Download CUDA toolkit
wget https://developer.download.nvidia.com/compute/cuda/11.8.0/local_installers/cuda_11.8.0_520.61.05_linux.run

# Install CUDA (run as root or with sudo)
sudo sh cuda_11.8.0_520.61.05_linux.run

# Add to PATH (add to ~/.bashrc or ~/.zshrc)
export PATH=/usr/local/cuda/bin${PATH:+:${PATH}}
export LD_LIBRARY_PATH=/usr/local/cuda/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
```

### Verify CUDA Installation
```bash
nvidia-smi
nvcc --version
```

### macOS CUDA Setup
CUDA is not officially supported on macOS. Use CPU-only mode or consider Linux/Windows for full functionality.

### Windows CUDA Setup
1. Download CUDA toolkit from NVIDIA website
2. Run installer as administrator
3. Add CUDA to system PATH
4. Verify installation with `nvcc --version`

---

## Building the Framework

### Standard Build (CPU Only)
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### CUDA-Enabled Build
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_CUDA=ON -DCUDA_ARCHITECTURES="75;86;89"
make -j$(nproc)
```

### Build Options
```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Enable all optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_OPTIMIZATIONS=ON

# Disable CUDA (CPU only)
cmake .. -DENABLE_CUDA=OFF

# Enable testing
cmake .. -DENABLE_TESTS=ON

# Enable documentation
cmake .. -DENABLE_DOCS=ON
```

### Build Artifacts
After successful build, you'll find:
- **Libraries:** `lib/libdublin_protocol.a` (static) or `lib/libdublin_protocol.so` (shared)
- **Executables:** `bin/` directory with demo programs
- **Headers:** `include/dublin/` with all public headers

---

## Running Demos

### Quick Start Demo
```bash
cd build
./bin/dublin_protocol_demo
```

### Individual Component Demos
```bash
# Quantum cognitive architecture
./bin/quantum_cognitive_demo

# Neural-quantum hybrid
./bin/quantum_neural_hybrid_demo

# Performance benchmarks
./bin/performance_benchmark_demo

# Consciousness modeling
./bin/consciousness_demo

# Emergent intelligence
./bin/emergent_intelligence_demo

# CUDA acceleration (if enabled)
./bin/cuda_dublin_protocol_demo

# DPL compiler
./bin/dpl_compiler_demo
```

### Expected Output
Each demo will display:
- Component initialization status
- Performance metrics
- Simulation results
- Analysis reports

---

## Troubleshooting

### Common Build Issues

#### CUDA Not Found
```
Error: CUDA toolkit not found
```
**Solution:**
```bash
# Check CUDA installation
nvcc --version
which nvcc

# Set CUDA path explicitly
export CUDA_HOME=/usr/local/cuda
export PATH=$CUDA_HOME/bin:$PATH
export LD_LIBRARY_PATH=$CUDA_HOME/lib64:$LD_LIBRARY_PATH

# Rebuild
cd build && rm -rf * && cmake .. -DENABLE_CUDA=ON
```

#### Compiler Errors
```
Error: C++17 not supported
```
**Solution:**
```bash
# GCC 7+ required
gcc --version

# Upgrade GCC if needed
sudo apt install gcc-9 g++-9
export CC=gcc-9 CXX=g++-9
```

#### Memory Issues
```
Error: CUDA out of memory
```
**Solution:**
- Reduce batch sizes in CUDA demos
- Close other GPU-intensive applications
- Use CPU-only mode: `cmake .. -DENABLE_CUDA=OFF`

### Runtime Issues

#### Demo Crashes
```bash
# Run with debug info
gdb ./bin/dublin_protocol_demo
run
bt  # Get backtrace on crash
```

#### Performance Issues
- Ensure you're using Release build: `-DCMAKE_BUILD_TYPE=Release`
- Check CPU/GPU utilization during runs
- Verify sufficient RAM (16GB+ recommended)

### CUDA-Specific Issues

#### GPU Not Detected
```bash
# Check GPU status
nvidia-smi

# Verify CUDA compatibility
nvidia-smi --query-gpu=compute_cap --format=csv
```

#### CUDA Kernel Failures
- Update GPU drivers
- Check CUDA version compatibility
- Try different CUDA architectures in cmake

---

## Development Setup

### IDE Configuration

#### Visual Studio Code
1. Install C++ extension
2. Configure IntelliSense:
```json
{
    "C_Cpp.default.includePath": [
        "${workspaceFolder}/src",
        "${workspaceFolder}/include",
        "/usr/local/cuda/include"
    ],
    "C_Cpp.default.compilerPath": "g++",
    "C_Cpp.default.cppStandard": "c++17"
}
```

#### CLion
1. Import project as CMake project
2. Set build type to Release
3. Enable CUDA support in settings

### Testing
```bash
# Build with tests enabled
cmake .. -DENABLE_TESTS=ON
make test

# Run specific tests
ctest -R quantum_test
ctest -R consciousness_test
```

### Documentation
```bash
# Generate API docs
doxygen Doxyfile

# View docs
firefox docs/html/index.html
```

### Contributing
1. Fork the repository
2. Create feature branch: `git checkout -b feature/new-component`
3. Make changes and add tests
4. Submit pull request

---

## Performance Optimization

### CPU Optimization
```bash
# Use optimized compiler flags
cmake .. -DCMAKE_CXX_FLAGS="-O3 -march=native -flto"

# Enable OpenMP for parallel processing
cmake .. -DENABLE_OPENMP=ON
```

### GPU Optimization
```bash
# Specify target GPU architecture
cmake .. -DCUDA_ARCHITECTURES="75;86;89"  # Turing, Ampere, Ada Lovelace

# Enable CUDA optimizations
cmake .. -DCUDA_NVCC_FLAGS="-O3 --use_fast_math"
```

### Memory Optimization
- Use memory pools for frequent allocations
- Implement proper CUDA memory management
- Profile memory usage with `nvidia-smi`

---

## Advanced Configuration

### Custom Build Options
```cmake
# In CMakeLists.txt or command line
-DMAX_AGENTS=2000              # Maximum number of agents
-DQUANTUM_DIMENSIONS=128       # Quantum state dimensions
-DNEURAL_HIDDEN_SIZE=512       # Neural network size
-DENABLE_PROFILING=ON          # Performance profiling
-DENABLE_LOGGING=ON            # Detailed logging
```

### Environment Variables
```bash
export DUBLIN_PROTOCOL_LOG_LEVEL=DEBUG
export DUBLIN_PROTOCOL_CUDA_DEVICE=0
export DUBLIN_PROTOCOL_MAX_MEMORY=8GB
```

---

## Getting Help

### Documentation
- **API Reference:** `docs/API_REFERENCE.md`
- **Master Report:** `DUBLIN_PROTOCOL_MASTER_REPORT.md`
- **Examples:** `tests/` directory

### Community Support
- **GitHub Issues:** Report bugs and request features
- **Discussions:** Ask questions and share experiences
- **Wiki:** Extended documentation and tutorials

### Professional Support
For enterprise deployment or custom development:
- Contact: dublin-protocol@research.org
- Consulting services available
- Custom training programs

---

## Verification

After successful installation, verify everything works:

```bash
# Run full test suite
cd build && ctest

# Run performance benchmark
./bin/performance_benchmark_demo

# Test CUDA functionality (if enabled)
./bin/cuda_dublin_protocol_demo

# Verify DPL compiler
./bin/dpl_compiler_demo
```

**Expected Result:** All demos run successfully with performance metrics displayed.

---

*This installation guide ensures you can successfully deploy and use the Dublin Protocol framework for computational consciousness research. For the latest updates, check the repository regularly.*

**Version 3.0 - October 16, 2025**

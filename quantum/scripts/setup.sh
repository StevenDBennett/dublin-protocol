#!/bin/bash

# 🚩 Dublin Protocol Setup Script
# Revolutionary framework setup for comrades

echo "🚀 Setting up Dublin Protocol Computational Revolution Framework"
echo "==============================================================="

# Check for required tools
echo ""
echo "🔧 Checking system requirements..."
if command -v g++ &> /dev/null; then
    echo "✅ GCC compiler found"
else
    echo "❌ GCC compiler not found. Please install g++"
    exit 1
fi

if command -v make &> /dev/null; then
    echo "✅ Make found"
else
    echo "❌ Make not found. Please install make"
    exit 1
fi

# Create necessary directories
echo ""
echo "📁 Creating directory structure..."
mkdir -p build

# Build the framework
echo ""
echo "🏗️ Building revolutionary framework..."
./build.sh

# Verify build
echo ""
echo "✅ Verification..."
if [ -f "tests/computational_agents_test" ]; then
    echo "✅ Computational agents test built successfully"
else
    echo "❌ Build failed"
    exit 1
fi

if [ -f "tests/benchmark" ]; then
    echo "✅ Performance benchmark built successfully"
else
    echo "❌ Benchmark build failed"
    exit 1
fi

echo ""
echo "🎉 SETUP COMPLETE!"
echo ""
echo "🚀 To begin the computational revolution:"
echo "   ./tests/computational_agents_test"
echo ""
echo "📊 To validate performance:"
echo "   ./tests/benchmark"
echo ""
echo "🌍 To test real-world applications:"
echo "   ./tests/real_world_agents_demo"
echo ""
echo "📖 Read WELCOME_COMRADE.md for complete introduction"
echo ""
echo "🎭 WELCOME TO THE COMPUTATIONAL REVOLUTION, COMRADE! 🚩"
echo ""
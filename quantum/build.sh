#!/bin/bash

# 🚩 Dublin Protocol Build Script
# Revolutionary computational framework compilation

echo "🚀 Building Dublin Protocol Computational Revolution Framework"
echo "=============================================================="

# Create build directory
mkdir -p build
cd build

# Compiler and flags
CXX="g++"
CXX_FLAGS="-std=c++23 -O3 -march=native -pthread"
INCLUDES="-I.. -I../src -I../src/core -I../src/agents -I../src/analytics -I../src/distributed -I../src/experiments -I../src/hybrid -I../src/optimized -I../src/quantum"

# Compile all tests
echo ""
echo "🧪 Compiling Revolutionary Tests..."

# Basic computational agents
$CXX $CXX_FLAGS $INCLUDES ../tests/computational_agents_test.cpp -o computational_agents_test

# Real-world applications
$CXX $CXX_FLAGS $INCLUDES ../tests/real_world_agents_demo.cpp -o real_world_agents_demo

# Carry agent demonstrations
$CXX $CXX_FLAGS $INCLUDES ../tests/carry_agent_simple.cpp -o carry_agent_simple
$CXX $CXX_FLAGS $INCLUDES ../tests/test_carry_fixed.cpp -o test_carry_fixed
$CXX $CXX_FLAGS $INCLUDES ../tests/test_carry_optimized.cpp -o test_carry_optimized

# Advanced features
$CXX $CXX_FLAGS $INCLUDES ../tests/advanced_agent_collaboration.cpp -o advanced_agent_collaboration
$CXX $CXX_FLAGS $INCLUDES ../tests/ai_ml_integration.cpp -o ai_ml_integration

# Performance benchmarks
$CXX $CXX_FLAGS $INCLUDES ../tests/benchmark.cpp -o benchmark
$CXX $CXX_FLAGS $INCLUDES ../tests/stress_test.cpp -o stress_test

# Experimental features
$CXX $CXX_FLAGS $INCLUDES ../tests/collaborative_intelligence_demo.cpp -o collaborative_intelligence_demo
$CXX $CXX_FLAGS $INCLUDES ../tests/hierarchical_consensus_experiment.cpp -o hierarchical_consensus_experiment
$CXX $CXX_FLAGS $INCLUDES ../tests/quantum_consensus_experiment.cpp -o quantum_consensus_experiment
$CXX $CXX_FLAGS $INCLUDES ../tests/mathematical_frontier_experiment.cpp -o mathematical_frontier_experiment
$CXX $CXX_FLAGS $INCLUDES ../tests/neural_consensus_network.cpp -o neural_consensus_network
$CXX $CXX_FLAGS $INCLUDES ../tests/consciousness_engine.cpp -o consciousness_engine
$CXX $CXX_FLAGS $INCLUDES ../tests/enhanced_consciousness_engine.cpp -o enhanced_consciousness_engine
$CXX $CXX_FLAGS $INCLUDES ../tests/consensus_language_model.cpp -o consensus_language_model
$CXX $CXX_FLAGS $INCLUDES ../tests/human_interactive_consensus_ai.cpp -o human_interactive_consensus_ai
$CXX $CXX_FLAGS $INCLUDES ../tests/consensual_training_ai.cpp -o consensual_training_ai
$CXX $CXX_FLAGS $INCLUDES ../tests/autonomous_consensus_ai.cpp -o autonomous_consensus_ai
$CXX $CXX_FLAGS $INCLUDES ../tests/conversational_anarchist_llm.cpp -o conversational_anarchist_llm
$CXX $CXX_FLAGS $INCLUDES ../tests/test_conversational_llm.cpp -o test_conversational_llm

# Demonstration programs
$CXX $CXX_FLAGS $INCLUDES ../tests/demonstrate_carry_standalone.cpp -o demonstrate_carry_standalone
$CXX $CXX_FLAGS $INCLUDES ../tests/demonstrate_carry_agent.cpp -o demonstrate_carry_agent
$CXX $CXX_FLAGS $INCLUDES ../tests/demonstrate_synthesis.cpp -o demonstrate_synthesis
$CXX $CXX_FLAGS $INCLUDES ../tests/synthesis_simple.cpp -o synthesis_simple

echo ""
echo "✅ Build Complete!"
echo ""
echo "🎯 Available Revolutionary Tests:"
echo "   ./computational_agents_test     - Basic computational agents"
echo "   ./real_world_agents_demo        - Real-world applications"
echo "   ./carry_agent_simple            - Carry agent demonstration"
echo "   ./benchmark                     - Performance validation"
echo "   ./advanced_agent_collaboration  - Advanced agent cooperation"
echo "   ./ai_ml_integration             - AI/ML integration"
echo ""
echo "🚀 To begin the revolution:"
echo "   ./computational_agents_test"
echo ""
echo "📖 Read WELCOME_COMRADE.md for complete introduction"
echo ""
echo "🎭 THE COMPUTATIONAL REVOLUTION AWAITS!"
echo ""

# Move executables to tests directory
mv computational_agents_test real_world_agents_demo carry_agent_simple test_carry_fixed test_carry_optimized advanced_agent_collaboration ai_ml_integration benchmark stress_test collaborative_intelligence_demo hierarchical_consensus_experiment quantum_consensus_experiment mathematical_frontier_experiment neural_consensus_network consciousness_engine enhanced_consciousness_engine consensus_language_model human_interactive_consensus_ai consensual_training_ai autonomous_consensus_ai conversational_anarchist_llm test_conversational_llm demonstrate_carry_standalone demonstrate_carry_agent demonstrate_synthesis synthesis_simple ../tests/

cd ..

# Dublin Protocol API Reference
## Complete Framework Documentation for Computational Consciousness Research

**Version:** 3.0  
**Date:** October 16, 2025  
**Framework:** Dublin Protocol - Consciousness Computation Framework  

---

## Table of Contents

1. [Core Classes](#core-classes)
2. [Quantum Components](#quantum-components)
3. [Neural Components](#neural-components)
4. [Consciousness Modeling](#consciousness-modeling)
5. [Emergent Intelligence](#emergent-intelligence)
6. [CUDA Acceleration](#cuda-acceleration)
7. [DPL Compiler](#dpl-compiler)
8. [Integration Framework](#integration-framework)

---

## Core Classes

### DublinProtocolIntegration

**Header:** `dublin_protocol_integration.hpp`  
**Namespace:** `dublin`  

The central orchestrator for the entire Dublin Protocol framework.

#### Constructor
```cpp
DublinProtocolIntegration();
```

#### Key Methods

**System Management:**
```cpp
void initialize_system();
void shutdown_system();
bool is_system_healthy() const;
```

**Component Registration:**
```cpp
void register_quantum_agent(std::shared_ptr<QuantumCognitiveAgent> agent);
void register_neural_agent(std::shared_ptr<NeuralCarryAgent> agent);
void register_hybrid_agent(std::shared_ptr<QuantumNeuralHybridAgent> agent);
```

**Task Processing:**
```cpp
TaskResult process_task(const Task& task);
std::vector<TaskResult> process_batch(const std::vector<Task>& tasks);
```

**Performance Monitoring:**
```cpp
SystemMetrics get_system_metrics() const;
PerformanceReport generate_performance_report() const;
```

**Evolution Control:**
```cpp
void start_evolution_cycle();
void pause_evolution_cycle();
void resume_evolution_cycle();
EvolutionStatus get_evolution_status() const;
```

---

## Quantum Components

### QuantumCognitiveAgent

**Header:** `quantum_cognitive_architecture.hpp`  
**Namespace:** `dublin`  

Quantum-enhanced cognitive agent with superposition reasoning.

#### Constructor
```cpp
QuantumCognitiveAgent(const std::string& name, size_t dimensions = 32);
```

#### Key Methods

**Cognitive Processing:**
```cpp
QuantumState process_input(const std::vector<double>& input);
std::vector<double> make_decision(const QuantumState& state);
```

**Learning:**
```cpp
void update_weights(const std::vector<double>& gradients, double learning_rate);
void adapt_to_environment(const EnvironmentState& env_state);
```

**Quantum Operations:**
```cpp
void apply_quantum_interference(const std::vector<double>& stimuli);
double measure_coherence() const;
QuantumSuperposition get_current_superposition() const;
```

### QuantumNeuralHybridAgent

**Header:** `quantum_neural_hybrid.hpp`  
**Namespace:** `dublin`  

Adaptive agent balancing quantum and neural processing.

#### Constructor
```cpp
QuantumNeuralHybridAgent(const std::string& name);
```

#### Key Methods

**Hybrid Processing:**
```cpp
HybridResult process_hybrid(const std::vector<double>& input);
void balance_processing_modes(double quantum_weight, double neural_weight);
```

**Adaptive Learning:**
```cpp
void optimize_balance(const PerformanceMetrics& metrics);
ProcessingMode get_optimal_mode(const TaskComplexity& complexity) const;
```

---

## Neural Components

### NeuralCarryAgent

**Header:** `quantum_neural_hybrid.hpp`  
**Namespace:** `qi` (Quantum Intelligence)  

Classical neural network agent with carry-forward memory.

#### Constructor
```cpp
NeuralCarryAgent(size_t input_size = 64, size_t hidden_size = 128, size_t output_size = 32);
```

#### Key Methods

**Neural Processing:**
```cpp
std::vector<double> forward(const std::vector<double>& input);
void backward(const std::vector<double>& target, double learning_rate);
```

**Memory Management:**
```cpp
void update_carry_memory(const std::vector<double>& new_memory);
std::vector<double> get_carry_state() const;
void reset_carry_memory();
```

---

## Consciousness Modeling

### ConsciousnessEvolutionSimulator

**Header:** `consciousness_pattern_analysis.hpp`  
**Namespace:** `dublin`  

Simulator for consciousness emergence and evolution.

#### Constructor
```cpp
ConsciousnessEvolutionSimulator(size_t num_states = 10, size_t num_features = 6);
```

#### Key Methods

**Simulation Control:**
```cpp
void simulate_evolution(size_t steps, double coherence_threshold);
void add_transition(const std::string& from_state, const std::string& to_state, double probability);
```

**Analysis:**
```cpp
std::string analyze_evolution() const;
ConsciousnessMetrics get_current_metrics() const;
std::vector<TransitionRecord> get_transition_history() const;
```

**Emergence Detection:**
```cpp
bool detect_emergence() const;
double measure_self_awareness() const;
CoherenceAnalysis analyze_coherence() const;
```

### ConsciousnessPatternAnalyzer

**Header:** `consciousness_pattern_analysis.hpp`  
**Namespace:** `dublin`  

Advanced pattern analysis for consciousness features.

#### Constructor
```cpp
ConsciousnessPatternAnalyzer();
```

#### Key Methods

**Pattern Analysis:**
```cpp
PatternResult analyze_patterns(const std::vector<ConsciousnessState>& states);
std::vector<Pattern> extract_patterns(const TimeSeries& data);
```

**Feature Extraction:**
```cpp
FeatureVector extract_features(const ConsciousnessState& state);
CorrelationMatrix compute_correlations(const std::vector<FeatureVector>& features);
```

---

## Emergent Intelligence

### EmergentIntelligenceCoordinator

**Header:** `emergent_intelligence_framework.hpp`  
**Namespace:** `dublin`  

Coordinator for collective intelligence emergence.

#### Constructor
```cpp
EmergentIntelligenceCoordinator(size_t num_agents = 16);
```

#### Key Methods

**Coordination:**
```cpp
void add_agent(std::shared_ptr<CognitiveAgent> agent);
CollectiveResult coordinate_agents(const Task& task);
```

**Emergence Analysis:**
```cpp
EmergenceMetrics analyze_emergence() const;
bool detect_collective_intelligence() const;
SynergyIndex measure_synergy() const;
```

### ConsensusLanguageModel

**Header:** `emergent_intelligence_framework.hpp`  
**Namespace:** `dublin`  

Language model for agent consensus and communication.

#### Constructor
```cpp
ConsensusLanguageModel(size_t vocab_size = 10000, size_t embedding_dim = 256);
```

#### Key Methods

**Language Processing:**
```cpp
std::vector<double> encode_message(const std::string& message);
std::string decode_message(const std::vector<double>& embedding);
```

**Consensus Building:**
```cpp
ConsensusResult build_consensus(const std::vector<std::string>& agent_messages);
AgreementLevel measure_agreement(const std::vector<ConsensusResult>& results);
```

---

## CUDA Acceleration

### CUDAQuantumCognitiveEngine

**Header:** `cuda_quantum_engine.cuh`  
**Namespace:** `dublin`  

GPU-accelerated quantum cognitive processing.

#### Constructor
```cpp
CUDAQuantumCognitiveEngine(size_t dims = 64, size_t batch = 32, size_t max_batch = 1024);
```

#### Key Methods

**Batch Processing:**
```cpp
std::vector<std::vector<float>> process_batch(const std::vector<std::vector<float>>& stimuli_batch);
```

**Performance Monitoring:**
```cpp
std::string get_performance_stats() const;
std::string get_memory_info() const;
```

### CUDAConsciousnessSimulator

**Header:** `cuda_quantum_engine.cuh`  
**Namespace:** `dublin`  

GPU-accelerated consciousness evolution simulation.

#### Constructor
```cpp
CUDAConsciousnessSimulator(size_t instances = 16, size_t steps = 1000);
```

#### Key Methods

**Simulation:**
```cpp
void run_simulation();
std::string get_simulation_report() const;
```

---

## DPL Compiler

### DublinProtocolCompiler

**Header:** `dublin_protocol_language.hpp`  
**Namespace:** `dublin`  

Compiler for Dublin Protocol Language (DPL).

#### Constructor
```cpp
DublinProtocolCompiler();
```

#### Key Methods

**Compilation:**
```cpp
std::string compile(const std::string& source_code);
std::string execute_compiled_code(const std::string& compiled_code);
```

### DPLDemonstration

**Header:** `dublin_protocol_language.hpp`  
**Namespace:** `dublin`  

Demonstration system for DPL compiler.

#### Static Methods
```cpp
static void demonstrate_dpl_compiler();
```

---

## Integration Framework

### PerformanceBenchmark

**Header:** `performance_benchmark.hpp`  
**Namespace:** `dublin`  

Comprehensive performance benchmarking suite.

#### Constructor
```cpp
PerformanceBenchmark();
```

#### Key Methods

**Benchmarking:**
```cpp
BenchmarkResult run_full_benchmark();
BenchmarkResult run_component_benchmark(ComponentType component);
```

**Analysis:**
```cpp
PerformanceAnalysis analyze_results(const BenchmarkResult& result);
std::string generate_report(const PerformanceAnalysis& analysis);
```

---

## Data Structures

### Task
```cpp
struct Task {
    std::string id;
    TaskType type;
    std::vector<double> input_data;
    TaskPriority priority;
    std::chrono::system_clock::time_point deadline;
};
```

### TaskResult
```cpp
struct TaskResult {
    std::string task_id;
    bool success;
    std::vector<double> output_data;
    double confidence;
    std::chrono::microseconds processing_time;
    std::string error_message;
};
```

### ConsciousnessState
```cpp
struct ConsciousnessState {
    std::string name;
    std::map<std::string, double> features; // attention, memory, emotion, etc.
    double coherence;
    double entropy;
    double self_awareness;
};
```

### QuantumState
```cpp
struct QuantumState {
    std::vector<std::complex<double>> amplitudes;
    size_t dimensions;
    double coherence_measure;
    std::vector<double> probability_distribution;
};
```

---

## Error Handling

### Exceptions
- `DublinProtocolException`: Base exception for framework errors
- `QuantumComputationError`: Quantum processing errors
- `ConsciousnessModelingError`: Consciousness simulation errors
- `CudaAccelerationError`: GPU acceleration errors
- `DPLCompilationError`: DPL compilation errors

### Error Codes
```cpp
enum class ErrorCode {
    SUCCESS = 0,
    INVALID_INPUT = 1,
    QUANTUM_STATE_COLLAPSE = 2,
    CONSCIOUSNESS_EMERGENCE_FAILED = 3,
    CUDA_MEMORY_ERROR = 4,
    DPL_SYNTAX_ERROR = 5
};
```

---

## Configuration

### System Configuration
```cpp
struct SystemConfig {
    size_t max_agents = 1000;
    size_t quantum_dimensions = 64;
    size_t neural_hidden_size = 256;
    double evolution_rate = 0.1;
    bool enable_cuda = true;
    size_t cuda_batch_size = 32;
};
```

### Agent Configuration
```cpp
struct AgentConfig {
    std::string name;
    AgentType type;
    size_t input_size;
    size_t output_size;
    double learning_rate = 0.01;
    bool enable_quantum = true;
    bool enable_neural = true;
};
```

---

## Performance Guidelines

### Memory Management
- Use smart pointers for all dynamic objects
- CUDA memory should be allocated once and reused
- Implement proper cleanup in destructors

### Thread Safety
- Core classes are not thread-safe by default
- Use external synchronization for multi-threaded access
- CUDA operations are asynchronous

### Optimization Tips
- Batch quantum operations for GPU efficiency
- Pre-allocate memory pools for frequent allocations
- Use const references for large data structures
- Profile performance-critical sections

---

## Future Extensions

### Planned APIs
- **Quantum Hardware Integration**: Direct quantum computer interfaces
- **Distributed Computing**: Multi-GPU and multi-node support
- **Real-time Visualization**: Live consciousness state rendering
- **Plugin Architecture**: Third-party component integration

### Research APIs
- **Consciousness Measurement**: Standardized consciousness metrics
- **Emergence Detection**: Advanced pattern recognition algorithms
- **Ethical AI Integration**: Consciousness-aware safety systems

---

*This API reference provides comprehensive documentation for the Dublin Protocol framework. For implementation examples, see the demonstration programs in the `tests/` directory.*

**Version 3.0 - October 16, 2025**

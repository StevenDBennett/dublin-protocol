# AI Tools for Dublin Protocol Research

Comprehensive AI tooling for Dublin Protocol computational universe research with DeepSeek and other AI providers.

## 🎯 Overview

This toolkit provides multiple ways to interact with AI systems for Dublin Protocol research:

1. **DeepSeek Unified Tool** - Primary DeepSeek interface with multiple usage modes
2. **AI Unified Framework** - Extensible framework for multiple AI providers
3. **Specialized Tools** - Individual tools for specific use cases

## 🚀 Quick Start

### 1. DeepSeek Unified Tool (Recommended)

```bash
# Direct API query
python deepseek_unified.py --api "Explain the 30ns computational barrier"

# Interactive chat with context saving
python deepseek_unified.py --interactive

# Code analysis
python deepseek_unified.py --analyze cosmic_computation.cpp
python deepseek_unified.py --analyze main.cpp --question "How to optimize?"

# Session management
python deepseek_unified.py --sessions
python deepseek_unified.py --switch research_session
```

### 2. AI Unified Framework (Multi-Provider)

```bash
# Single provider query
python ai_unified.py --query "Quantum gravity from computation" --provider deepseek

# Cross-validation across providers
python ai_unified.py --query "30ns barrier" --cross-validate

# List available providers
python ai_unified.py --list-providers
```

## 🛠️ Tool Descriptions

### `deepseek_unified.py`
**Primary DeepSeek interface with full feature set:**
- ✅ Direct API calls
- ✅ Interactive chat with tool calling
- ✅ Context saving across sessions
- ✅ Code analysis and file operations
- ✅ Session management
- ✅ Dublin Protocol context integration

**Usage Examples:**
```bash
# Direct research queries
python deepseek_unified.py --api "What computational patterns emerge at 30ns?"

# Interactive research session
python deepseek_unified.py --interactive --session quantum_research

# Analyze specific implementations
python deepseek_unified.py --analyze consciousness_computer.cpp

# With additional context files
python deepseek_unified.py --api "Review this approach" --context DUBLIN_PROTOCOL_GUIDE.md
```

### `ai_unified.py`
**Extensible framework for multiple AI providers:**
- ✅ DeepSeek, OpenAI, Anthropic support
- ✅ Cross-validation across providers
- ✅ Response saving and comparison
- ✅ Dublin Protocol context
- ✅ Easy provider registration

**Usage Examples:**
```bash
# Compare AI perspectives
python ai_unified.py --query "Consciousness = Entropy × Complexity" --cross-validate

# Specific provider query
python ai_unified.py --query "Test prediction" --provider openai --model gpt-4

# Custom context
python ai_unified.py --query "Analyze" --context "Focus on computational implementations"
```

### Specialized Tools

- `deepseek_working.py` - Simple OpenAI-compatible API
- `deepseek_final.py` - Tool-calling with Anthropic-compatible API
- `deepseek_integration.py` - Dublin Protocol integration
- `deepseek_interactive.py` - Interactive chat with context

## 🔧 Environment Setup

### 1. Virtual Environment
```bash
# Activate virtual environment
source venv/bin/activate

# Or use activation script
./activate_venv.sh
```

### 2. API Keys
Set environment variables:
```bash
export DEEPSEEK_API_KEY="your_deepseek_key"
export OPENAI_API_KEY="your_openai_key"  # Optional
export ANTHROPIC_API_KEY="your_anthropic_key"  # Optional
```

### 3. Dependencies
All required packages are automatically installed when tools run.

## 📁 File Structure

```
tools/
├── deepseek_unified.py      # 🎯 Primary unified tool
├── ai_unified.py           # 🔬 Multi-provider framework
├── deepseek_working.py     # Simple API
├── deepseek_final.py       # Tool-calling
├── deepseek_integration.py # Dublin Protocol integration
├── deepseek_interactive.py # Interactive chat
├── chat_context/           # Saved conversations
│   ├── default_conversation.json
│   └── session_*.json
└── ai_context/            # Cross-validation results
    └── provider_*.json
```

## 🎯 Dublin Protocol Integration

All tools include Dublin Protocol context:
- 30ns computational light speed barrier
- XOR/AND computational duality
- Consciousness mathematics
- Multiverse Darwinism
- Human-AI collaboration framework

## 🔄 Session Management

### Conversation Persistence
- Conversations automatically saved in `chat_context/`
- Multiple sessions supported
- Context maintained across tool restarts

### Session Commands
```bash
# List sessions
python deepseek_unified.py --sessions

# Switch session
python deepseek_unified.py --switch quantum_research

# Clear history (in interactive mode)
clear

# Force save
save
```

## 🛠️ Tool Calling Capabilities

Available tools in interactive mode:
- `execute_bash` - Run shell commands
- `read_file` - Read file contents
- `write_file` - Write to files
- `list_files` - List directory contents
- `git_status` - Check git status

## 🔬 Research Workflow

### 1. Initial Exploration
```bash
python deepseek_unified.py --interactive --session exploration
```

### 2. Code Analysis
```bash
python deepseek_unified.py --analyze cosmic_computation.cpp
```

### 3. Cross-Validation
```bash
python ai_unified.py --query "Test hypothesis" --cross-validate
```

### 4. Implementation
Use tool calling to modify code and run experiments.

## 🚨 Troubleshooting

### Common Issues

1. **API Key Not Set**
   ```bash
   Error: DEEPSEEK_API_KEY environment variable not set
   ```
   **Solution:** Set the environment variable

2. **Virtual Environment**
   ```bash
   python: command not found
   ```
   **Solution:** Use `python3` or activate virtual environment

3. **Package Installation**
   ```bash
   ModuleNotFoundError
   ```
   **Solution:** Tools auto-install dependencies on first run

### Debug Mode
Add `--debug` flag to any tool for verbose output.

## 📚 Advanced Usage

### Custom Context Integration
```bash
python deepseek_unified.py --api "Your query" --context file1.md file2.cpp
```

### Batch Processing
```bash
# Process multiple queries
for query in "query1" "query2" "query3"; do
    python deepseek_unified.py --api "$query"
done
```

### Integration with Other Tools
Use tool calling to integrate with:
- Code compilation and testing
- Data analysis pipelines
- Experiment automation

## 🎉 Next Steps

1. **Start with** `deepseek_unified.py --interactive` for collaborative research
2. **Use** `--analyze` for code review and optimization
3. **Leverage** cross-validation for robust hypothesis testing
4. **Extend** with custom tools for specific research needs

---

**🌌 Dublin Protocol Research Tools - Ready for Computational Universe Exploration**
# Chat Context Directory

This directory stores persistent conversation context for DeepSeek interactive chats.

## Files

- `{session_name}_conversation.json` - Full conversation history in JSON format
- Session files maintain context across multiple chat sessions

## Usage

### Start Interactive Session
```bash
python tools/deepseek_interactive.py

# With specific session name
python tools/deepseek_interactive.py --session quantum_analysis
```

### Single Message
```bash
python tools/deepseek_interactive.py --message "Analyze the 30ns barrier"
```

## Session Management

- **Multiple Sessions**: Use different session names for different topics
- **Persistent Context**: Conversations are saved automatically
- **Clear History**: Type 'clear' during chat to reset
- **Manual Save**: Type 'save' to force save

## Example Sessions

- `dublin_protocol` - General Dublin Protocol research
- `quantum_analysis` - Quantum mechanics from computation
- `consciousness_research` - Consciousness mathematics
- `multiverse_simulation` - Multiverse rule evolution

## Context Preservation

The system maintains:
- Full conversation history
- Tool execution results
- Dublin Protocol research context
- Cross-session memory
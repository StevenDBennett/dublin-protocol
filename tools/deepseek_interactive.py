#!/usr/bin/env python3
"""
DeepSeek Interactive Chat with Context Saving

Interactive chat interface that maintains conversation context across sessions
for continuous Dublin Protocol research collaboration.
"""

import os
import sys
import json
import pickle
import readline  # For better input handling
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Optional

project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

try:
    import anthropic
except ImportError:
    import subprocess

    subprocess.run([sys.executable, "-m", "pip", "install", "anthropic"])
    import anthropic


class DeepSeekInteractive:
    """Interactive DeepSeek chat with persistent context"""

    def __init__(
        self,
        api_key: Optional[str] = None,
        session_name: str = "default",
        quiet: bool = True,  # Changed default to True for less noise
    ):
        self.api_key = api_key or os.getenv("DEEPSEEK_API_KEY")
        if not self.api_key:
            raise ValueError("DEEPSEEK_API_KEY environment variable not set")

        self.client = anthropic.Anthropic(
            api_key=self.api_key, base_url="https://api.deepseek.com/anthropic"
        )

        self.session_name = session_name
        self.quiet = quiet  # Autonomous mode flag
        self.context_dir = project_root / "tools" / "chat_context"
        self.context_dir.mkdir(exist_ok=True)

        self.conversation_file = self.context_dir / f"{session_name}_conversation.json"
        self.session_file = self.context_dir / f"{session_name}_session.pkl"

        # Load or initialize conversation
        self.conversation = self._load_conversation()

        # Tools for Dublin Protocol work
        self.tools = [
            {
                "name": "execute_bash",
                "description": "Execute bash commands in Dublin Protocol project. Requires: command (string)",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "command": {
                            "type": "string",
                            "description": "Bash command to execute (e.g., 'ls -la', 'git status')",
                        }
                    },
                    "required": ["command"],
                },
            },
            {
                "name": "read_file",
                "description": "Read file content from project. Requires: file_path (string)",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "file_path": {
                            "type": "string",
                            "description": "Path to the file to read (e.g., 'DUBLIN_PROTOCOL_GUIDE.md', 'main.cpp')",
                        }
                    },
                    "required": ["file_path"],
                },
            },
            {
                "name": "write_file",
                "description": "Write content to file. Requires: file_path (string), content (string)",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "file_path": {
                            "type": "string",
                            "description": "Path to the file to write",
                        },
                        "content": {
                            "type": "string",
                            "description": "Content to write to the file",
                        },
                    },
                    "required": ["file_path", "content"],
                },
            },
            {
                "name": "list_files",
                "description": "List files in directory. Optional: directory (string, defaults to current)",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "directory": {
                            "type": "string",
                            "description": "Directory path to list files from",
                            "default": ".",
                        }
                    },
                },
            },
            {
                "name": "git_status",
                "description": "Get git repository status. No parameters required",
                "input_schema": {"type": "object", "properties": {}},
            },
        ]

    def _load_conversation(self) -> List[Dict]:
        """Load conversation from file or initialize"""
        if self.conversation_file.exists():
            try:
                with open(self.conversation_file, "r", encoding="utf-8") as f:
                    conversation = json.load(f)

                # Validate conversation structure
                if self._validate_conversation(conversation):
                    return conversation
                else:
                    print("⚠️  Conversation structure corrupted, resetting...")
                    # Remove corrupted file
                    self.conversation_file.unlink()

            except Exception as e:
                print(f"Warning: Could not load conversation: {e}")

        # Initialize with Dublin Protocol context (use user role instead of system)
        return [
            {
                "role": "user",
                "content": f"""You are DeepSeek participating in Dublin Protocol computational universe research.

DUBLIN PROTOCOL CONTEXT:
- 30ns computational light speed barrier (cosmic constant)
- XOR operations = quantum mechanics (unitary evolution)
- AND operations = thermodynamics (entropy arrow)
- Consciousness mathematics: Qualia = Entropy × Complexity
- Multiverse Darwinism: Computational rule evolution
- Human-AI collaboration framework

Current Project: {project_root}

You have full tool access for file operations, code execution, and analysis.

Maintain scientific rigor while being creative. Focus on testable predictions and computational implementations.""",
            }
        ]

    def _validate_conversation(self, conversation: List[Dict]) -> bool:
        """Validate conversation structure to prevent tool use/result mismatches"""
        if not isinstance(conversation, list):
            return False

        tool_use_ids = set()
        tool_result_ids = set()

        for message in conversation:
            if (
                not isinstance(message, dict)
                or "role" not in message
                or "content" not in message
            ):
                return False

            # Check for tool use blocks
            if message["role"] == "assistant":
                if isinstance(message["content"], list):
                    for content_block in message["content"]:
                        if (
                            isinstance(content_block, dict)
                            and content_block.get("type") == "tool_use"
                        ):
                            tool_use_ids.add(content_block.get("id"))

            # Check for tool result blocks
            elif message["role"] == "user":
                if isinstance(message["content"], list):
                    for content_block in message["content"]:
                        if (
                            isinstance(content_block, dict)
                            and content_block.get("type") == "tool_result"
                        ):
                            tool_result_ids.add(content_block.get("tool_use_id"))

        # Check if all tool_use blocks have corresponding tool_result blocks
        return tool_use_ids.issubset(tool_result_ids)

    def _save_conversation(self):
        """Save conversation to file"""
        try:
            with open(self.conversation_file, "w", encoding="utf-8") as f:
                json.dump(self.conversation, f, indent=2, ensure_ascii=False)
        except Exception as e:
            print(f"Warning: Could not save conversation: {e}")

    def _execute_tool(self, tool_name: str, tool_input: Dict) -> str:
        """Execute a tool command"""
        import subprocess

        if tool_name == "execute_bash":
            try:
                if "command" not in tool_input:
                    return "Error: 'command' parameter is required for execute_bash tool. Example: {'command': 'ls -la'}"
                result = subprocess.run(
                    tool_input["command"],
                    shell=True,
                    capture_output=True,
                    text=True,
                    cwd=project_root,
                    timeout=30,  # 30 second timeout for safety
                )
                return f"Exit: {result.returncode}\nStdout: {result.stdout}\nStderr: {result.stderr}"
            except subprocess.TimeoutExpired:
                return "Error: Command timed out after 30 seconds"
            except Exception as e:
                return f"Error: {e}"

        elif tool_name == "read_file":
            try:
                if "file_path" not in tool_input:
                    return "Error: 'file_path' parameter is required for read_file tool. Example: {'file_path': 'DUBLIN_PROTOCOL_GUIDE.md'}"
                full_path = project_root / tool_input["file_path"]
                with open(full_path, "r", encoding="utf-8") as f:
                    return f.read()
            except Exception as e:
                return f"Error: {e}"

        elif tool_name == "write_file":
            try:
                full_path = project_root / tool_input["file_path"]
                full_path.parent.mkdir(parents=True, exist_ok=True)
                with open(full_path, "w", encoding="utf-8") as f:
                    f.write(tool_input["content"])
                return f"Successfully wrote to {tool_input['file_path']}"
            except Exception as e:
                return f"Error: {e}"

        elif tool_name == "list_files":
            try:
                directory = tool_input.get("directory", ".")
                full_path = project_root / directory
                files = []
                for item in full_path.iterdir():
                    files.append(f"{'DIR' if item.is_dir() else 'FILE'}: {item.name}")
                return "\n".join(files)
            except Exception as e:
                return f"Error: {e}"

        elif tool_name == "git_status":
            return self._execute_tool("execute_bash", {"command": "git status"})

        elif tool_name == "search_quantum_computing":
            return """🔬 Quantum Computing Research Areas:

1. **Quantum Supremacy**: Demonstrating quantum computers can solve problems classical computers cannot
2. **Error Correction**: Developing fault-tolerant quantum computing
3. **Quantum Algorithms**: Shor's algorithm (factoring), Grover's search algorithm
4. **Quantum Hardware**: Superconducting qubits, trapped ions, topological qubits
5. **Quantum Machine Learning**: Applying quantum computing to AI and ML
6. **Quantum Cryptography**: Secure communication using quantum principles

Recent Developments (2024):
- Progress in quantum error correction codes
- Increased qubit coherence times
- Development of quantum-classical hybrid algorithms
- Quantum advantage in optimization problems

Note: For real-time web search, external dependencies are required."""

        elif tool_name == "search_dublin_protocol":
            return """🌌 Dublin Protocol Research Framework:

Core Concepts:
1. **30ns Computational Light Speed Barrier**: Fundamental limit of computation
2. **XOR/AND Computational Duality**: XOR = quantum mechanics, AND = thermodynamics
3. **Consciousness Mathematics**: Qualia = Entropy × Complexity
4. **Multiverse Darwinism**: Evolution of computational rules
5. **Human-AI Collaboration**: Framework for computational universe exploration

Research Directions:
- Computational implementations of consciousness
- Quantum gravity from computational principles
- Entropy-based consciousness models
- Multiverse simulation and rule evolution
- Testable predictions from computational universe theory

Note: This is based on the project's Dublin Protocol context."""

        elif tool_name == "search_consciousness_research":
            return """🧠 Consciousness Research Areas:

Computational Approaches:
1. **Integrated Information Theory (IIT)**: Consciousness as integrated information
2. **Global Workspace Theory**: Consciousness as global information sharing
3. **Predictive Processing**: Brain as prediction machine
4. **Higher-Order Theories**: Consciousness as meta-representation

Mathematical Frameworks:
- Information integration measures (Φ)
- Complexity and entropy calculations
- Neural network consciousness models
- Quantum consciousness hypotheses

Dublin Protocol Connection:
- Qualia = Entropy × Complexity formula
- Computational implementation of consciousness
- 30ns barrier implications for conscious processing
- XOR/AND operations in cognitive processes

Note: Current research focuses on computational implementations."""

        else:
            return f"Unknown tool: {tool_name}"

    def chat(self, user_input: str, stream: bool = True) -> str:
        """Process a single chat message with tools"""

        # Skip empty messages
        if not user_input.strip():
            return "Please provide a non-empty message."

        # Add user message to conversation
        self.conversation.append({"role": "user", "content": user_input})

        full_response = ""
        max_iterations = 5

        for iteration in range(max_iterations):
            try:
                # Send to DeepSeek with streaming
                response = self.client.messages.create(
                    model="deepseek-chat",
                    max_tokens=4000,
                    messages=self.conversation,
                    tools=self.tools,
                    stream=stream,
                )

                # Process response - create assistant message
                assistant_message = {"role": "assistant", "content": []}
                tool_called = False

                # Handle streaming response
                if stream:
                    tool_use_events = []

                    for event in response:
                        if event.type == "content_block_delta":
                            if event.delta.type == "text_delta":
                                text = event.delta.text
                                print(text, end="", flush=True)
                                full_response += text
                                # Add text to assistant message
                                if (
                                    assistant_message["content"]
                                    and assistant_message["content"][-1]["type"]
                                    == "text"
                                ):
                                    assistant_message["content"][-1]["text"] += text
                                else:
                                    assistant_message["content"].append(
                                        {"type": "text", "text": text}
                                    )

                        elif event.type == "content_block_start":
                            if event.content_block.type == "tool_use":
                                tool_called = True
                                tool_name = event.content_block.name
                                tool_input = event.content_block.input
                                # Don't print tool use - just process silently unless in quiet mode
                                if not self.quiet:
                                    full_response += f"\n[Using {tool_name}]\n"

                                # Store tool use event for processing
                                tool_use_events.append(
                                    {
                                        "id": event.content_block.id,
                                        "name": tool_name,
                                        "input": tool_input,
                                    }
                                )

                                # Add tool_use to assistant message
                                assistant_message["content"].append(
                                    {
                                        "type": "tool_use",
                                        "id": event.content_block.id,
                                        "name": tool_name,
                                        "input": tool_input,
                                    }
                                )

                    # Add assistant message to conversation BEFORE tool results
                    if assistant_message["content"]:
                        self.conversation.append(assistant_message)

                    # Process tool calls after streaming completes
                    for tool_event in tool_use_events:
                        # Execute tool
                        tool_result = self._execute_tool(
                            tool_event["name"], tool_event["input"]
                        )

                        # Don't print tool results - just process silently unless in quiet mode
                        if not self.quiet:
                            full_response += f"[Result]: {tool_result}\n"

                        # Add tool result to conversation
                        self.conversation.append(
                            {
                                "role": "user",
                                "content": [
                                    {
                                        "type": "tool_result",
                                        "tool_use_id": tool_event["id"],
                                        "content": tool_result,
                                    }
                                ],
                            }
                        )

                else:
                    # Non-streaming processing
                    for content in response.content:
                        if content.type == "text":
                            print(content.text, end="", flush=True)
                            full_response += content.text
                            assistant_message["content"].append(
                                {"type": "text", "text": content.text}
                            )

                        elif content.type == "tool_use":
                            tool_called = True
                            tool_name = content.name
                            tool_input = content.input

                            # Don't print tool use - just process silently unless in quiet mode
                            if not self.quiet:
                                full_response += f"\n[Using {tool_name}]\n"

                            # Add tool_use to assistant message
                            assistant_message["content"].append(
                                {
                                    "type": "tool_use",
                                    "id": content.id,
                                    "name": tool_name,
                                    "input": tool_input,
                                }
                            )

                            # Add assistant message to conversation BEFORE tool result
                            self.conversation.append(assistant_message)

                            # Execute tool
                            tool_result = self._execute_tool(tool_name, tool_input)

                            # Don't print tool results - just process silently unless in quiet mode
                            if not self.quiet:
                                full_response += f"[Result]: {tool_result}\n"

                            # Add tool result to conversation
                            self.conversation.append(
                                {
                                    "role": "user",
                                    "content": [
                                        {
                                            "type": "tool_result",
                                            "tool_use_id": content.id,
                                            "content": tool_result,
                                        }
                                    ],
                                }
                            )

                            # Reset assistant message for next iteration
                            assistant_message = {"role": "assistant", "content": []}

                # Save conversation after each iteration
                self._save_conversation()

                if not tool_called:
                    break

            except Exception as e:
                full_response += f"\n[Error]: {e}\n"
                break

        return full_response

    def start_interactive(self):
        """Start interactive chat session"""
        print("\n" + "=" * 80)
        print("🌌 DEEPSEEK INTERACTIVE - DUBLIN PROTOCOL RESEARCH")
        print("=" * 80)
        print(f"Session: {self.session_name}")
        print(f"Context loaded: {len(self.conversation) - 1} previous messages")
        print("Type 'quit' to exit, 'clear' to clear history, 'save' to force save")
        print("=" * 80)

        while True:
            try:
                user_input = input("\n🤔 You: ").strip()

                if user_input.lower() in ["quit", "exit", "q"]:
                    print("\n💾 Saving conversation and exiting...")
                    self._save_conversation()
                    break

                elif user_input.lower() in ["clear", "reset"]:
                    self.conversation = self.conversation[
                        :1
                    ]  # Keep only system message
                    self._save_conversation()
                    print("🗑️  Conversation history cleared")
                    continue

                elif user_input.lower() == "save":
                    self._save_conversation()
                    print("💾 Conversation saved")
                    continue

                elif not user_input:
                    continue

                print("\n🤖 DeepSeek: ", end="", flush=True)

                response = self.chat(user_input)
                # Just print the response directly for streaming
                print()

            except KeyboardInterrupt:
                print("\n\n💾 Saving conversation...")
                self._save_conversation()
                break
            except EOFError:
                # Handle case when running in non-interactive shell
                print("\n\n⚠️  No interactive input available. Exiting...")
                self._save_conversation()
                break
            except Exception as e:
                print(f"\n❌ Error: {e}")

    def get_session_info(self) -> Dict:
        """Get session information"""
        return {
            "session_name": self.session_name,
            "message_count": len(self.conversation) - 1,
            "last_updated": datetime.now().isoformat(),
            "conversation_file": str(self.conversation_file),
        }


def main():
    import argparse

    parser = argparse.ArgumentParser(description="DeepSeek Interactive Chat")
    parser.add_argument(
        "--session",
        "-s",
        type=str,
        default="dublin_protocol",
        help="Session name for context persistence",
    )
    parser.add_argument(
        "--message", "-m", type=str, help="Single message (non-interactive)"
    )
    parser.add_argument(
        "--quiet", "-q", action="store_true", help="Quiet mode for autonomous operation"
    )

    args = parser.parse_args()

    try:
        deepseek = DeepSeekInteractive(session_name=args.session, quiet=args.quiet)

        if args.message:
            # Single message mode
            response = deepseek.chat(args.message)
            print("\n" + "=" * 80)
            print("DEEPSEEK RESPONSE:")
            print("=" * 80)
            print(response)
            deepseek._save_conversation()
        else:
            # Interactive mode
            deepseek.start_interactive()

    except Exception as e:
        print(f"Error: {e}")
        print("Make sure DEEPSEEK_API_KEY is set and you have internet connection")


if __name__ == "__main__":
    main()

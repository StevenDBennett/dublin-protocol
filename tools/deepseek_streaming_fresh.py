#!/usr/bin/env python3
"""
DeepSeek Streaming Interactive Chat with Context Saving

Interactive chat interface with streaming responses for real-time interaction
while maintaining conversation context across sessions.
"""

import os
import sys
import json
import pickle
import readline  # For better input handling
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Optional, Generator

project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

try:
    import anthropic
except ImportError:
    import subprocess

    subprocess.run([sys.executable, "-m", "pip", "install", "anthropic"])
    import anthropic


class DeepSeekStreaming:
    """Interactive DeepSeek chat with streaming responses and persistent context"""

    def __init__(self, api_key: Optional[str] = None, session_name: str = "default"):
        self.api_key = api_key or os.getenv("DEEPSEEK_API_KEY")
        if not self.api_key:
            raise ValueError("DEEPSEEK_API_KEY environment variable not set")

        self.client = anthropic.Anthropic(
            api_key=self.api_key, base_url="https://api.deepseek.com/anthropic"
        )

        self.session_name = session_name
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
                "description": "Execute bash commands in Dublin Protocol project",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "command": {"type": "string", "description": "Bash command"}
                    },
                    "required": ["command"],
                },
            },
            {
                "name": "read_file",
                "description": "Read file content from project",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "file_path": {"type": "string", "description": "File path"}
                    },
                    "required": ["file_path"],
                },
            },
            {
                "name": "write_file",
                "description": "Write content to file",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "file_path": {"type": "string", "description": "File path"},
                        "content": {"type": "string", "description": "File content"},
                    },
                    "required": ["file_path", "content"],
                },
            },
            {
                "name": "list_files",
                "description": "List files in directory",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "directory": {
                            "type": "string",
                            "description": "Directory path",
                            "default": ".",
                        }
                    },
                },
            },
            {
                "name": "git_status",
                "description": "Get git repository status",
                "input_schema": {"type": "object", "properties": {}},
            },
        ]

    def _load_conversation(self) -> List[Dict]:
        """Load conversation from file or initialize - convert system to user roles"""
        if self.conversation_file.exists():
            try:
                with open(self.conversation_file, "r", encoding="utf-8") as f:
                    conversation = json.load(f)
                    # Convert any system messages to user messages
                    for msg in conversation:
                        if msg.get("role") == "system":
                            msg["role"] = "user"
                    return conversation
            except Exception as e:
                print(f"Warning: Could not load conversation: {e}")

        # Initialize with Dublin Protocol context - use user role
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
                )
                return f"Exit: {result.returncode}\nStdout: {result.stdout}\nStderr: {result.stderr}"
            except Exception as e:
                return f"Error: {e}"

        elif tool_name == "read_file":
            try:
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

        else:
            return f"Unknown tool: {tool_name}"

    def stream_chat(self, user_input: str) -> Generator[str, None, None]:
        """Process a single chat message with streaming response"""

        # Skip empty messages
        if not user_input.strip():
            yield "Please provide a non-empty message."
            return

        # Add user message to conversation
        self.conversation.append({"role": "user", "content": user_input})

        full_response = ""
        max_iterations = 5

        for iteration in range(max_iterations):
            try:
                # Send to DeepSeek with streaming
                with self.client.messages.stream(
                    model="deepseek-chat",
                    max_tokens=4000,
                    messages=self.conversation,
                    tools=self.tools,
                ) as stream:

                    assistant_message = {"role": "assistant", "content": []}
                    tool_called = False
                    current_tool_use = None

                    for event in stream:
                        if event.type == "content_block_delta":
                            if event.delta.type == "text_delta":
                                text = event.delta.text
                                yield text
                                full_response += text

                                # Add to assistant message
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
                                current_tool_use = {
                                    "type": "tool_use",
                                    "id": event.content_block.id,
                                    "name": event.content_block.name,
                                    "input": event.content_block.input,
                                }
                                assistant_message["content"].append(current_tool_use)

                                yield f"\n[Tool: {event.content_block.name} with {event.content_block.input}]\n"

                    # After streaming completes, handle tool execution if needed
                    if tool_called and current_tool_use:
                        # Add assistant message to conversation BEFORE tool result
                        self.conversation.append(assistant_message)

                        # Execute tool
                        tool_result = self._execute_tool(
                            current_tool_use["name"], current_tool_use["input"]
                        )

                        yield f"[Tool Result]: {tool_result}\n"

                        # Add tool result to conversation
                        self.conversation.append(
                            {
                                "role": "user",
                                "content": [
                                    {
                                        "type": "tool_result",
                                        "tool_use_id": current_tool_use["id"],
                                        "content": tool_result,
                                    }
                                ],
                            }
                        )

                    else:
                        # No tools called, just add the assistant message
                        self.conversation.append(assistant_message)
                        break

                # Save conversation after each iteration
                self._save_conversation()

                if not tool_called:
                    break

            except Exception as e:
                yield f"\n[Error]: {e}\n"
                break

    def start_interactive(self):
        """Start interactive chat session with streaming"""
        print("\n" + "=" * 80)
        print("🌌 DEEPSEEK STREAMING - DUBLIN PROTOCOL RESEARCH")
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

                # Stream the response
                for chunk in self.stream_chat(user_input):
                    print(chunk, end="", flush=True)
                print()  # New line after streaming completes

            except KeyboardInterrupt:
                print("\n\n💾 Saving conversation...")
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

    parser = argparse.ArgumentParser(description="DeepSeek Streaming Interactive Chat")
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

    args = parser.parse_args()

    try:
        deepseek = DeepSeekStreaming(session_name=args.session)

        if args.message:
            # Single message mode with streaming
            print("\n" + "=" * 80)
            print("DEEPSEEK STREAMING RESPONSE:")
            print("=" * 80)
            for chunk in deepseek.stream_chat(args.message):
                print(chunk, end="", flush=True)
            print()
            deepseek._save_conversation()
        else:
            # Interactive mode with streaming
            deepseek.start_interactive()

    except Exception as e:
        print(f"Error: {e}")
        print("Make sure DEEPSEEK_API_KEY is set and you have internet connection")


if __name__ == "__main__":
    main()

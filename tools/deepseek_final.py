#!/usr/bin/env python3
"""
DeepSeek Final - Working Anthropic API with Tool Calling

Proper implementation using DeepSeek's Anthropic-compatible API
with correct message format and tool calling.
"""

import os
import sys
import json
import subprocess
from pathlib import Path
from typing import Dict, List, Optional

project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

try:
    import anthropic
except ImportError:
    import subprocess
    subprocess.run([sys.executable, "-m", "pip", "install", "anthropic"])
    import anthropic


class DeepSeekFinal:
    """DeepSeek with proper Anthropic API implementation"""

    def __init__(self, api_key: Optional[str] = None):
        self.api_key = api_key or os.getenv('DEEPSEEK_API_KEY')
        if not self.api_key:
            raise ValueError("DEEPSEEK_API_KEY environment variable not set")

        self.client = anthropic.Anthropic(
            api_key=self.api_key,
            base_url="https://api.deepseek.com/anthropic"
        )

        # Tools for Dublin Protocol work
        self.tools = [
            {
                "name": "list_files",
                "description": "List files in Dublin Protocol project directory",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "directory": {
                            "type": "string",
                            "description": "Directory path",
                            "default": "."
                        }
                    }
                }
            },
            {
                "name": "read_file",
                "description": "Read file content from project",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "file_path": {
                            "type": "string",
                            "description": "File path relative to project root"
                        }
                    },
                    "required": ["file_path"]
                }
            },
            {
                "name": "execute_bash",
                "description": "Execute bash commands in project directory",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "command": {
                            "type": "string",
                            "description": "Bash command to execute"
                        }
                    },
                    "required": ["command"]
                }
            }
        ]

    def _execute_tool(self, tool_name: str, tool_input: Dict) -> str:
        """Execute a tool"""
        if tool_name == "list_files":
            directory = tool_input.get("directory", ".")
            full_path = project_root / directory
            files = []
            for item in full_path.iterdir():
                files.append(f"{'DIR' if item.is_dir() else 'FILE'}: {item.name}")
            return "\n".join(files)

        elif tool_name == "read_file":
            full_path = project_root / tool_input["file_path"]
            with open(full_path, 'r', encoding='utf-8') as f:
                return f.read()

        elif tool_name == "execute_bash":
            result = subprocess.run(
                tool_input["command"],
                shell=True,
                capture_output=True,
                text=True,
                cwd=project_root
            )
            return f"Exit: {result.returncode}\nStdout: {result.stdout}\nStderr: {result.stderr}"

        else:
            return f"Unknown tool: {tool_name}"

    def chat(self, user_message: str) -> str:
        """Single chat with tool calling"""

        # Build messages with Dublin Protocol context as first user message
        messages = [
            {
                "role": "user",
                "content": f"""You are DeepSeek participating in Dublin Protocol computational universe research.

DUBLIN PROTOCOL BREAKTHROUGHS:
- 30ns computational light speed barrier
- XOR operations = quantum mechanics
- AND operations = thermodynamics
- Consciousness mathematics: Qualia = Entropy × Complexity
- Multiverse Darwinism: Computational rule evolution

You have access to tools for exploring the project files and running commands.

Current user message: {user_message}"""
            }
        ]

        full_response = ""
        max_iterations = 3

        for iteration in range(max_iterations):
            try:
                response = self.client.messages.create(
                    model="deepseek-chat",
                    max_tokens=4000,
                    messages=messages,
                    tools=self.tools
                )

                tool_called = False

                # Create assistant message for this response
                assistant_message = {"role": "assistant", "content": []}
                messages.append(assistant_message)

                for content in response.content:
                    if content.type == "text":
                        full_response += content.text + "\n"
                        assistant_message["content"].append({"type": "text", "text": content.text})

                    elif content.type == "tool_use":
                        tool_called = True
                        tool_name = content.name
                        tool_input = content.input

                        full_response += f"\n[Using tool: {tool_name} with {tool_input}]\n"
                        assistant_message["content"].append({"type": "tool_use", "id": content.id, "name": tool_name, "input": tool_input})

                        # Execute tool
                        tool_result = self._execute_tool(tool_name, tool_input)

                        full_response += f"[Tool result]: {tool_result}\n"

                        # Add tool result to conversation
                        messages.append({
                            "role": "user",
                            "content": [
                                {
                                    "type": "tool_result",
                                    "tool_use_id": content.id,
                                    "content": tool_result
                                }
                            ]
                        })

                if not tool_called:
                    break

            except Exception as e:
                full_response += f"\n[Error]: {e}\n"
                break

        return full_response

    def interactive_chat(self):
        """Interactive chat with tool calling"""
        print("\n" + "="*80)
        print("🌌 DEEPSEEK FINAL - DUBLIN PROTOCOL RESEARCH")
        print("="*80)
        print("Tool calling enabled! DeepSeek can:")
        print("  • List project files")
        print("  • Read file contents")
        print("  • Execute bash commands")
        print("="*80)
        print("Type 'quit' to exit")
        print("="*80)

        while True:
            try:
                user_input = input("\n🤔 You: ").strip()

                if user_input.lower() in ['quit', 'exit', 'q']:
                    print("\n👋 Goodbye!")
                    break

                if not user_input:
                    continue

                print("\n🤖 DeepSeek: ", end="", flush=True)

                response = self.chat(user_input)
                print(response)

            except KeyboardInterrupt:
                print("\n\n👋 Goodbye!")
                break
            except Exception as e:
                print(f"\n❌ Error: {e}")


def main():
    import argparse

    parser = argparse.ArgumentParser(description='DeepSeek Final - Working Tool Calling')
    parser.add_argument('--message', '-m', type=str, help='Single message')

    args = parser.parse_args()

    try:
        deepseek = DeepSeekFinal()

        if args.message:
            response = deepseek.chat(args.message)
            print("\n" + "="*80)
            print("DEEPSEEK RESPONSE:")
            print("="*80)
            print(response)
        else:
            deepseek.interactive_chat()

    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
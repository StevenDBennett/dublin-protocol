#!/usr/bin/env python3
"""
DeepSeek with Anthropic-Compatible Tool Calling

Uses DeepSeek's Anthropic-compatible API endpoint that supports proper tool calling.
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
    print("Installing anthropic...")
    subprocess.run([sys.executable, "-m", "pip", "install", "anthropic"])
    import anthropic


class DeepSeekAnthropicTools:
    """DeepSeek using Anthropic-compatible API with tool calling"""

    def __init__(self, api_key: Optional[str] = None):
        self.api_key = api_key or os.getenv('DEEPSEEK_API_KEY')
        if not self.api_key:
            raise ValueError("DEEPSEEK_API_KEY environment variable not set")

        # Use Anthropic client with DeepSeek's Anthropic-compatible endpoint
        self.client = anthropic.Anthropic(
            api_key=self.api_key,
            base_url="https://api.deepseek.com/anthropic"
        )

        # Define tools for Dublin Protocol work
        self.tools = [
            {
                "name": "execute_bash",
                "description": "Execute bash commands in the Dublin Protocol project directory",
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
            },
            {
                "name": "read_file",
                "description": "Read file content from Dublin Protocol project",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "file_path": {
                            "type": "string",
                            "description": "Path to file relative to project root"
                        }
                    },
                    "required": ["file_path"]
                }
            },
            {
                "name": "list_files",
                "description": "List files in Dublin Protocol project directory",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "directory": {
                            "type": "string",
                            "description": "Directory path (default: current directory)",
                            "default": "."
                        }
                    }
                }
            },
            {
                "name": "git_status",
                "description": "Get git status of Dublin Protocol repository",
                "input_schema": {
                    "type": "object",
                    "properties": {}
                }
            },
            {
                "name": "compile_cpp",
                "description": "Compile C++ files in Dublin Protocol project",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "file_path": {
                            "type": "string",
                            "description": "Path to C++ file to compile"
                        }
                    },
                    "required": ["file_path"]
                }
            }
        ]

    def _execute_bash(self, command: str) -> str:
        """Execute bash command"""
        try:
            result = subprocess.run(
                command,
                shell=True,
                capture_output=True,
                text=True,
                cwd=project_root
            )
            return f"Exit code: {result.returncode}\nStdout: {result.stdout}\nStderr: {result.stderr}"
        except Exception as e:
            return f"Error executing command: {e}"

    def _read_file(self, file_path: str) -> str:
        """Read file content"""
        try:
            full_path = project_root / file_path
            with open(full_path, 'r', encoding='utf-8') as f:
                return f.read()
        except Exception as e:
            return f"Error reading file: {e}"

    def _list_files(self, directory: str = ".") -> str:
        """List files in directory"""
        try:
            full_path = project_root / directory
            files = []
            for item in full_path.iterdir():
                files.append(f"{'DIR' if item.is_dir() else 'FILE'}: {item.name}")
            return "\n".join(files)
        except Exception as e:
            return f"Error listing files: {e}"

    def _git_status(self) -> str:
        """Get git status"""
        return self._execute_bash("git status")

    def _compile_cpp(self, file_path: str) -> str:
        """Compile C++ file"""
        try:
            full_path = project_root / file_path
            output_name = full_path.stem
            command = f"g++ -std=c++23 -O3 {full_path} -o {output_name}"
            return self._execute_bash(command)
        except Exception as e:
            return f"Error compiling: {e}"

    def execute_tool(self, tool_name: str, tool_input: Dict) -> str:
        """Execute a specific tool"""
        if tool_name == "execute_bash":
            return self._execute_bash(tool_input["command"])
        elif tool_name == "read_file":
            return self._read_file(tool_input["file_path"])
        elif tool_name == "list_files":
            directory = tool_input.get("directory", ".")
            return self._list_files(directory)
        elif tool_name == "git_status":
            return self._git_status()
        elif tool_name == "compile_cpp":
            return self._compile_cpp(tool_input["file_path"])
        else:
            return f"Unknown tool: {tool_name}"

    def chat_with_tools(self, prompt: str, max_iterations: int = 5) -> str:
        """Chat with DeepSeek using proper tool calling"""

        messages = [
            {
                "role": "user",
                "content": f"""You are DeepSeek participating in Dublin Protocol computational universe research.

Dublin Protocol Context:
- 30ns computational light speed barrier
- XOR operations = quantum mechanics
- AND operations = thermodynamics
- Consciousness mathematics: Qualia = Entropy × Complexity
- Multiverse Darwinism

You have access to tools for file operations, bash commands, and code compilation.

Prompt: {prompt}"""
            }
        ]

        full_conversation = f"PROMPT: {prompt}\n\n"

        for iteration in range(max_iterations):
            try:
                # Send message with tools
                response = self.client.messages.create(
                    model="deepseek-chat",
                    max_tokens=4000,
                    messages=messages,
                    tools=self.tools
                )

                # Add response to conversation
                assistant_message = {"role": "assistant", "content": []}
                messages.append(assistant_message)

                # Process response content
                for content in response.content:
                    if content.type == "text":
                        full_conversation += f"\nDEEPSEEK: {content.text}\n"
                        assistant_message["content"].append({"type": "text", "text": content.text})

                    elif content.type == "tool_use":
                        # Execute tool
                        tool_name = content.name
                        tool_input = content.input

                        full_conversation += f"\nTOOL CALL: {tool_name} with {tool_input}\n"
                        assistant_message["content"].append({"type": "tool_use", "id": content.id, "name": tool_name, "input": tool_input})

                        # Execute tool
                        tool_result = self.execute_tool(tool_name, tool_input)

                        full_conversation += f"TOOL RESULT: {tool_result}\n"

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

                # Check if we should continue
                if not any(content.type == "tool_use" for content in response.content):
                    # No more tool calls, conversation complete
                    break

            except Exception as e:
                full_conversation += f"\nERROR: {e}\n"
                break

        return full_conversation


def main():
    import argparse
    parser = argparse.ArgumentParser(description='DeepSeek with Anthropic Tool Calling')
    parser.add_argument('prompt', type=str, help='Prompt for DeepSeek')
    parser.add_argument('--iterations', '-i', type=int, default=5, help='Max tool iterations')

    args = parser.parse_args()

    try:
        deepseek = DeepSeekAnthropicTools()

        print("\n" + "="*80)
        print("DEEPSEEK WITH ANTHROPIC TOOLS - DUBLIN PROTOCOL")
        print("="*80)

        result = deepseek.chat_with_tools(args.prompt, args.iterations)
        print(result)

    except Exception as e:
        print(f"Error: {e}")
        print("\nTroubleshooting:")
        print("- Ensure DEEPSEEK_API_KEY is set")
        print("- Check internet connection")
        print("- Verify DeepSeek API supports Anthropic-compatible endpoints")


if __name__ == "__main__":
    main()
#!/usr/bin/env python3
"""
Simple DeepSeek Chat with Tool Calling
A simplified version that doesn't use conversation history for tool calls
"""

import os
import sys
from pathlib import Path

project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

try:
    import anthropic
except ImportError:
    import subprocess
    subprocess.run([sys.executable, "-m", "pip", "install", "anthropic"])
    import anthropic


def execute_tool(tool_name: str, tool_input: dict) -> str:
    """Execute a tool command"""
    import subprocess

    if tool_name == "execute_bash":
        try:
            if "command" not in tool_input:
                return "Error: 'command' parameter is required for execute_bash tool"
            result = subprocess.run(
                tool_input["command"],
                shell=True,
                capture_output=True,
                text=True,
                cwd=project_root
            )
            return f"Exit: {result.returncode}\nStdout: {result.stdout}\nStderr: {result.stderr}"
        except Exception as e:
            return f"Error: {e}"

    elif tool_name == "read_file":
        try:
            if "file_path" not in tool_input:
                return "Error: 'file_path' parameter is required for read_file tool"
            full_path = project_root / tool_input["file_path"]
            with open(full_path, 'r', encoding='utf-8') as f:
                return f.read()
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

    else:
        return f"Unknown tool: {tool_name}"


def chat_with_tools(message: str) -> str:
    """Send a message to DeepSeek and handle tool calls"""

    client = anthropic.Anthropic(
        api_key=os.getenv('DEEPSEEK_API_KEY'),
        base_url="https://api.deepseek.com/anthropic"
    )

    tools = [
        {
            "name": "execute_bash",
            "description": "Execute bash commands in Dublin Protocol project. REQUIRED: command parameter. Example: {'command': 'ls -la'}",
            "input_schema": {
                "type": "object",
                "properties": {
                    "command": {"type": "string", "description": "Bash command to execute (e.g., 'ls -la', 'git status')"}
                },
                "required": ["command"]
            }
        },
        {
            "name": "read_file",
            "description": "Read file content from project. REQUIRED: file_path parameter. Example: {'file_path': 'DUBLIN_PROTOCOL_GUIDE.md'}",
            "input_schema": {
                "type": "object",
                "properties": {
                    "file_path": {"type": "string", "description": "Path to the file to read (e.g., 'DUBLIN_PROTOCOL_GUIDE.md', 'main.cpp')"}
                },
                "required": ["file_path"]
            }
        },
        {
            "name": "list_files",
            "description": "List files in directory. OPTIONAL: directory parameter (defaults to current directory)",
            "input_schema": {
                "type": "object",
                "properties": {
                    "directory": {"type": "string", "description": "Directory path to list files from", "default": "."}
                }
            }
        }
    ]

    # Initial message
    messages = [
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

Maintain scientific rigor while being creative. Focus on testable predictions and computational implementations.

User message: {message}"""
        }
    ]

    max_iterations = 5
    full_response = ""

    for iteration in range(max_iterations):
        try:
            response = client.messages.create(
                model="deepseek-chat",
                max_tokens=4000,
                messages=messages,
                tools=tools
            )

            tool_called = False

            for content in response.content:
                if content.type == "text":
                    print(content.text, end="", flush=True)
                    full_response += content.text

                elif content.type == "tool_use":
                    tool_called = True
                    tool_name = content.name
                    tool_input = content.input

                    print(f"\n[Tool: {tool_name} with {tool_input}]\n", end="", flush=True)
                    full_response += f"\n[Tool: {tool_name} with {tool_input}]\n"

                    # Execute tool
                    tool_result = execute_tool(tool_name, tool_input)

                    print(f"[Tool Result]: {tool_result}\n", end="", flush=True)
                    full_response += f"[Tool Result]: {tool_result}\n"

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


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Simple DeepSeek Chat with Tools')
    parser.add_argument('--message', '-m', type=str, required=True, help='Message to send to DeepSeek')

    args = parser.parse_args()

    try:
        print("\n" + "="*80)
        print("🌌 DEEPSEEK SIMPLE CHAT - DUBLIN PROTOCOL RESEARCH")
        print("="*80)
        print(f"Message: {args.message}")
        print("="*80)
        print("\n🤖 DeepSeek: ", end="", flush=True)

        response = chat_with_tools(args.message)
        print("\n" + "="*80)
        print("COMPLETED")
        print("="*80)

    except Exception as e:
        print(f"\n❌ Error: {e}")
        print("Make sure DEEPSEEK_API_KEY is set and you have internet connection")
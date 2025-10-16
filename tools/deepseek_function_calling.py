#!/usr/bin/env python3
"""
DeepSeek Function Calling with OpenAI-compatible API

Uses DeepSeek's official function calling API with strict mode
for better parameter handling and reliability.
"""

import os
import sys
import json
import argparse
from pathlib import Path
from typing import Dict, List, Optional, Any

project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

try:
    from openai import OpenAI
except ImportError:
    import subprocess
    subprocess.run([sys.executable, "-m", "pip", "install", "openai"])
    from openai import OpenAI


class DeepSeekFunctionCalling:
    """DeepSeek function calling with OpenAI-compatible API"""

    def __init__(self, api_key: Optional[str] = None, use_strict_mode: bool = True):
        self.api_key = api_key or os.getenv('DEEPSEEK_API_KEY')
        if not self.api_key:
            raise ValueError("DEEPSEEK_API_KEY environment variable not set")

        # Use beta endpoint for strict mode
        base_url = "https://api.deepseek.com/beta" if use_strict_mode else "https://api.deepseek.com"

        self.client = OpenAI(
            api_key=self.api_key,
            base_url=base_url
        )

        self.use_strict_mode = use_strict_mode
        self.tools = self._get_tools()

    def _get_tools(self) -> List[Dict]:
        """Define tools with strict mode support"""

        tools = [
            {
                "type": "function",
                "function": {
                    "name": "execute_bash",
                    "description": "Execute bash commands in Dublin Protocol project",
                    "strict": self.use_strict_mode,
                    "parameters": {
                        "type": "object",
                        "properties": {
                            "command": {
                                "type": "string",
                                "description": "Bash command to execute"
                            }
                        },
                        "required": ["command"],
                        "additionalProperties": False
                    }
                }
            },
            {
                "type": "function",
                "function": {
                    "name": "read_file",
                    "description": "Read file content from project",
                    "strict": self.use_strict_mode,
                    "parameters": {
                        "type": "object",
                        "properties": {
                            "file_path": {
                                "type": "string",
                                "description": "Path to the file to read (e.g., 'DUBLIN_PROTOCOL_GUIDE.md')"
                            }
                        },
                        "required": ["file_path"],
                        "additionalProperties": False
                    }
                }
            },
            {
                "type": "function",
                "function": {
                    "name": "write_file",
                    "description": "Write content to file",
                    "strict": self.use_strict_mode,
                    "parameters": {
                        "type": "object",
                        "properties": {
                            "file_path": {
                                "type": "string",
                                "description": "Path to the file to write"
                            },
                            "content": {
                                "type": "string",
                                "description": "Content to write to the file"
                            }
                        },
                        "required": ["file_path", "content"],
                        "additionalProperties": False
                    }
                }
            },
            {
                "type": "function",
                "function": {
                    "name": "list_files",
                    "description": "List files in directory",
                    "strict": self.use_strict_mode,
                    "parameters": {
                        "type": "object",
                        "properties": {
                            "directory": {
                                "type": "string",
                                "description": "Directory path to list files from",
                                "default": "."
                            }
                        },
                        "additionalProperties": False
                    }
                }
            },
            {
                "type": "function",
                "function": {
                    "name": "git_status",
                    "description": "Get git repository status",
                    "strict": self.use_strict_mode,
                    "parameters": {
                        "type": "object",
                        "properties": {},
                        "additionalProperties": False
                    }
                }
            }
        ]

        return tools

    def _execute_function(self, function_name: str, function_args: Dict) -> str:
        """Execute a function command"""
        import subprocess

        if function_name == "execute_bash":
            try:
                result = subprocess.run(
                    function_args["command"],
                    shell=True,
                    capture_output=True,
                    text=True,
                    cwd=project_root
                )
                return f"Exit: {result.returncode}\nStdout: {result.stdout}\nStderr: {result.stderr}"
            except Exception as e:
                return f"Error: {e}"

        elif function_name == "read_file":
            try:
                full_path = project_root / function_args["file_path"]
                with open(full_path, 'r', encoding='utf-8') as f:
                    return f.read()
            except Exception as e:
                return f"Error: {e}"

        elif function_name == "write_file":
            try:
                full_path = project_root / function_args["file_path"]
                full_path.parent.mkdir(parents=True, exist_ok=True)
                with open(full_path, 'w', encoding='utf-8') as f:
                    f.write(function_args["content"])
                return f"Successfully wrote to {function_args['file_path']}"
            except Exception as e:
                return f"Error: {e}"

        elif function_name == "list_files":
            try:
                directory = function_args.get("directory", ".")
                full_path = project_root / directory
                files = []
                for item in full_path.iterdir():
                    files.append(f"{'DIR' if item.is_dir() else 'FILE'}: {item.name}")
                return "\n".join(files)
            except Exception as e:
                return f"Error: {e}"

        elif function_name == "git_status":
            return self._execute_function("execute_bash", {"command": "git status"})

        else:
            return f"Unknown function: {function_name}"

    def chat(self, messages: List[Dict], max_iterations: int = 5) -> str:
        """Process chat with function calling"""

        conversation = messages.copy()
        full_response = ""

        for iteration in range(max_iterations):
            try:
                # Send to DeepSeek
                response = self.client.chat.completions.create(
                    model="deepseek-chat",
                    messages=conversation,
                    tools=self.tools
                )

                message = response.choices[0].message

                # Print the response
                if message.content:
                    print(f"🤖 DeepSeek: {message.content}")
                    full_response += message.content + "\n"

                # Check for function calls
                if message.tool_calls:
                    for tool_call in message.tool_calls:
                        function_name = tool_call.function.name
                        function_args = json.loads(tool_call.function.arguments)

                        print(f"🔧 Function: {function_name} with {function_args}")

                        # Add assistant message to conversation
                        conversation.append({
                            "role": "assistant",
                            "content": message.content,
                            "tool_calls": [
                                {
                                    "id": tool_call.id,
                                    "type": "function",
                                    "function": {
                                        "name": function_name,
                                        "arguments": tool_call.function.arguments
                                    }
                                }
                            ]
                        })

                        # Execute function
                        function_result = self._execute_function(function_name, function_args)
                        print(f"📋 Result: {function_result}")

                        # Add function result to conversation
                        conversation.append({
                            "role": "tool",
                            "tool_call_id": tool_call.id,
                            "content": function_result
                        })

                    # Continue conversation with function results
                    continue
                else:
                    # No function calls, conversation complete
                    break

            except Exception as e:
                full_response += f"\n[Error]: {e}\n"
                break

        return full_response

    def single_query(self, query: str) -> str:
        """Single query with Dublin Protocol context"""

        messages = [
            {
                "role": "system",
                "content": """You are DeepSeek participating in Dublin Protocol computational universe research.

DUBLIN PROTOCOL BREAKTHROUGHS:
- 30ns computational light speed barrier (cosmic constant)
- XOR operations = quantum mechanics (unitary evolution)
- AND operations = thermodynamics (entropy arrow)
- Consciousness mathematics: Qualia = Entropy × Complexity
- Multiverse Darwinism: Computational rule evolution

You have access to tools for file operations, code execution, and analysis.

Focus on computational implementations, testable predictions, and cross-validation."""
            },
            {
                "role": "user",
                "content": query
            }
        ]

        return self.chat(messages)

    def start_interactive(self):
        """Start interactive chat session"""
        print("\n" + "="*80)
        print("🌌 DEEPSEEK FUNCTION CALLING - DUBLIN PROTOCOL RESEARCH")
        print("="*80)
        print(f"Mode: {'Strict' if self.use_strict_mode else 'Standard'}")
        print("Type 'quit' to exit")
        print("="*80)

        messages = [
            {
                "role": "system",
                "content": """You are DeepSeek participating in Dublin Protocol computational universe research.

DUBLIN PROTOCOL BREAKTHROUGHS:
- 30ns computational light speed barrier (cosmic constant)
- XOR operations = quantum mechanics (unitary evolution)
- AND operations = thermodynamics (entropy arrow)
- Consciousness mathematics: Qualia = Entropy × Complexity
- Multiverse Darwinism: Computational rule evolution

You have access to tools for file operations, code execution, and analysis.

Focus on computational implementations, testable predictions, and cross-validation."""
            }
        ]

        while True:
            try:
                user_input = input("\n🤔 You: ").strip()

                if user_input.lower() in ['quit', 'exit', 'q']:
                    print("\n👋 Exiting...")
                    break

                elif not user_input:
                    continue

                # Add user message
                messages.append({"role": "user", "content": user_input})

                # Get response
                self.chat(messages)

            except KeyboardInterrupt:
                print("\n\n👋 Exiting...")
                break
            except Exception as e:
                print(f"\n❌ Error: {e}")


def main():
    parser = argparse.ArgumentParser(description='DeepSeek Function Calling')

    parser.add_argument('--query', '-q', type=str, help='Single query')
    parser.add_argument('--interactive', '-i', action='store_true', help='Interactive mode')
    parser.add_argument('--no-strict', action='store_true', help='Disable strict mode')

    args = parser.parse_args()

    try:
        deepseek = DeepSeekFunctionCalling(use_strict_mode=not args.no_strict)

        if args.query:
            # Single query mode
            print("\n" + "="*80)
            print("DEEPSEEK RESPONSE:")
            print("="*80)
            response = deepseek.single_query(args.query)
            print(response)

        elif args.interactive:
            # Interactive mode
            deepseek.start_interactive()

        else:
            # Show usage
            print("""
🌌 DEEPSEEK FUNCTION CALLING - USAGE MODES:

1. Single Query:
   python deepseek_function_calling.py --query "Your query here"
   python deepseek_function_calling.py --query "Read DUBLIN_PROTOCOL_GUIDE.md"

2. Interactive Mode:
   python deepseek_function_calling.py --interactive

3. Standard Mode (no strict):
   python deepseek_function_calling.py --interactive --no-strict

Environment: Make sure DEEPSEEK_API_KEY is set
""")

    except Exception as e:
        print(f"Error: {e}")
        print("Make sure DEEPSEEK_API_KEY environment variable is set")


if __name__ == "__main__":
    main()
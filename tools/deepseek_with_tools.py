#!/usr/bin/env python3
"""
DeepSeek with Tool Capabilities

A bridge that allows DeepSeek to use tools like bash, file operations, etc.
This works by:
1. You send prompts to DeepSeek
2. DeepSeek requests tools in its response
3. This script executes the tools and sends results back
"""

import os
import sys
import json
import subprocess
import argparse
from pathlib import Path
from typing import Dict, List, Optional

# Add project root to path
project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

try:
    from openai import OpenAI
except ImportError:
    print("Installing openai...")
    subprocess.run([sys.executable, "-m", "pip", "install", "openai"])
    from openai import OpenAI


class DeepSeekWithTools:
    """DeepSeek with tool execution capabilities"""

    def __init__(self, api_key: Optional[str] = None):
        self.api_key = api_key or os.getenv('DEEPSEEK_API_KEY')
        if not self.api_key:
            raise ValueError("DEEPSEEK_API_KEY environment variable not set")

        self.client = OpenAI(
            api_key=self.api_key,
            base_url="https://api.deepseek.com"
        )

        # Tool registry
        self.tools = {
            "bash": self._execute_bash,
            "read_file": self._read_file,
            "write_file": self._write_file,
            "list_files": self._list_files,
            "git_status": self._git_status,
            "compile_cpp": self._compile_cpp
        }

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

    def _write_file(self, file_path: str, content: str) -> str:
        """Write to file"""
        try:
            full_path = project_root / file_path
            full_path.parent.mkdir(parents=True, exist_ok=True)
            with open(full_path, 'w', encoding='utf-8') as f:
                f.write(content)
            return f"Successfully wrote to {file_path}"
        except Exception as e:
            return f"Error writing file: {e}"

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

    def _parse_tool_requests(self, response: str) -> List[Dict]:
        """Parse tool requests from DeepSeek response"""
        # Look for tool request patterns like:
        # TOOL: bash "ls -la"
        # TOOL: read_file "cosmic_computation.cpp"

        tool_requests = []
        lines = response.split('\n')

        for line in lines:
            line = line.strip()
            if line.startswith('TOOL:'):
                parts = line[5:].strip().split(' ', 1)
                if len(parts) >= 2:
                    tool_name = parts[0].strip()
                    tool_args = parts[1].strip()

                    # Handle quoted arguments
                    if tool_args.startswith('"') and tool_args.endswith('"'):
                        tool_args = tool_args[1:-1]

                    tool_requests.append({
                        'tool': tool_name,
                        'args': tool_args
                    })

        return tool_requests

    def chat_with_tools(self, prompt: str, max_iterations: int = 3) -> str:
        """Chat with DeepSeek using tools"""

        conversation = [
            {
                "role": "system",
                "content": """You are DeepSeek participating in Dublin Protocol research.

You can use tools by formatting your requests like:
TOOL: tool_name "arguments"

Available tools:
- bash "command" - Execute bash command
- read_file "path" - Read file content
- write_file "path" "content" - Write to file
- list_files "directory" - List files
- git_status - Get git status
- compile_cpp "file.cpp" - Compile C++ file

After tool execution, you'll see the results and can continue the conversation.

Focus on Dublin Protocol computational universe research."""
            },
            {
                "role": "user",
                "content": prompt
            }
        ]

        full_conversation = ""

        for iteration in range(max_iterations):
            # Get DeepSeek response
            try:
                response = self.client.chat.completions.create(
                    model="deepseek-chat",
                    messages=conversation,
                    temperature=0.7,
                    max_tokens=4000
                )

                deepseek_response = response.choices[0].message.content
                full_conversation += f"\n\nDEEPSEEK: {deepseek_response}"

                # Check for tool requests
                tool_requests = self._parse_tool_requests(deepseek_response)

                if not tool_requests:
                    # No tools requested, conversation complete
                    return full_conversation

                # Execute tools and build response
                tool_results = []
                for tool_req in tool_requests:
                    tool_name = tool_req['tool']
                    tool_args = tool_req['args']

                    if tool_name in self.tools:
                        result = self.tools[tool_name](tool_args)
                        tool_results.append(f"TOOL {tool_name} {tool_args}: {result}")
                    else:
                        tool_results.append(f"Unknown tool: {tool_name}")

                tool_response = "\n".join(tool_results)
                full_conversation += f"\n\nTOOL RESULTS:\n{tool_response}"

                # Continue conversation with tool results
                conversation.extend([
                    {"role": "assistant", "content": deepseek_response},
                    {"role": "user", "content": f"Tool results:\n{tool_response}\n\nPlease continue your analysis."}
                ])

            except Exception as e:
                return f"Error in conversation: {e}"

        return full_conversation


def main():
    parser = argparse.ArgumentParser(description='DeepSeek with Tool Capabilities')
    parser.add_argument('prompt', type=str, help='Prompt for DeepSeek')
    parser.add_argument('--iterations', '-i', type=int, default=3, help='Max tool iterations')

    args = parser.parse_args()

    try:
        deepseek = DeepSeekWithTools()
        result = deepseek.chat_with_tools(args.prompt, args.iterations)

        print("\n" + "="*80)
        print("DEEPSEEK WITH TOOLS - CONVERSATION:")
        print("="*80)
        print(result)

    except Exception as e:
        print(f"Error: {e}")
        print("Make sure DEEPSEEK_API_KEY is set and you have an active internet connection")


if __name__ == "__main__":
    main()
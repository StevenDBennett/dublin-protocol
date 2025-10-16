#!/usr/bin/env python3
"""
Unified DeepSeek AI Tool

A comprehensive tool for:
1. Direct DeepSeek API access
2. Interactive chat with context saving
3. Extensible framework for other AI APIs
4. Dublin Protocol research integration

Usage modes:
- Direct API: python deepseek_unified.py --api "your query"
- Interactive: python deepseek_unified.py --interactive
- File analysis: python deepseek_unified.py --analyze file.cpp
- Session management: python deepseek_unified.py --sessions
"""

import os
import sys
import json
import argparse
import readline
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Optional, Any

project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

# Try multiple API clients
try:
    import anthropic
except ImportError:
    import subprocess

    subprocess.run([sys.executable, "-m", "pip", "install", "anthropic"])
    import anthropic

try:
    from openai import OpenAI
except ImportError:
    import subprocess

    subprocess.run([sys.executable, "-m", "pip", "install", "openai"])
    from openai import OpenAI


class UnifiedAITool:
    """Unified AI tool for multiple APIs and usage modes"""

    def __init__(
        self,
        api_key: Optional[str] = None,
        session_name: str = "default",
        quiet: bool = True,  # Changed default to True for less noise
    ):
        self.api_key = api_key or os.getenv("DEEPSEEK_API_KEY")
        if not self.api_key:
            raise ValueError("DEEPSEEK_API_KEY environment variable not set")

        # Initialize both API clients
        self.anthropic_client = anthropic.Anthropic(
            api_key=self.api_key, base_url="https://api.deepseek.com/anthropic"
        )

        self.openai_client = OpenAI(
            api_key=self.api_key, base_url="https://api.deepseek.com"
        )

        self.session_name = session_name
        self.quiet = quiet  # Autonomous mode flag
        self.context_dir = project_root / "tools" / "chat_context"
        self.context_dir.mkdir(exist_ok=True)

        self.conversation_file = self.context_dir / f"{session_name}_conversation.json"
        self.sessions_file = self.context_dir / "sessions.json"

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
                            "description": "Bash command to execute",
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
                            "description": "Path to file to read",
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
                            "description": "Path to file to write",
                        },
                        "content": {
                            "type": "string",
                            "description": "Content to write",
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
                            "description": "Directory path",
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

                    # Check for orphaned tool_use blocks and clean them
                    cleaned_conversation = self._clean_orphaned_tool_calls(conversation)
                    if len(cleaned_conversation) != len(conversation):
                        print(
                            f"⚠️  Cleaned {len(conversation) - len(cleaned_conversation)} orphaned tool calls from conversation"
                        )

                    return cleaned_conversation
            except Exception as e:
                print(f"Warning: Could not load conversation: {e}")

        # Initialize with Dublin Protocol context
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

    def _clean_orphaned_tool_calls(self, conversation: List[Dict]) -> List[Dict]:
        """Remove orphaned tool_use blocks without corresponding tool_result blocks"""
        cleaned = []
        i = 0

        while i < len(conversation):
            message = conversation[i]

            # Check if this message contains tool_use blocks
            has_tool_use = False
            if isinstance(message.get("content"), list):
                for content in message["content"]:
                    if content.get("type") == "tool_use":
                        has_tool_use = True
                        break

            if has_tool_use:
                # Check if next message has corresponding tool_result
                if i + 1 < len(conversation):
                    next_message = conversation[i + 1]
                    has_tool_result = False

                    if isinstance(next_message.get("content"), list):
                        for content in next_message["content"]:
                            if content.get("type") == "tool_result":
                                has_tool_result = True
                                break

                    if has_tool_result:
                        # Keep both tool_use and tool_result messages
                        cleaned.append(message)
                        cleaned.append(next_message)
                        i += 2
                    else:
                        # Skip orphaned tool_use message
                        print(f"⚠️  Removing orphaned tool_use at position {i}")
                        i += 1
                else:
                    # Skip orphaned tool_use at end of conversation
                    print(f"⚠️  Removing orphaned tool_use at end of conversation")
                    i += 1
            else:
                # Keep non-tool messages
                cleaned.append(message)
                i += 1

        return cleaned

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

    # ===== DIRECT API METHODS =====

    def direct_api_call(self, prompt: str, context_files: List[str] = None) -> str:
        """Direct API call using OpenAI-compatible interface"""

        # Build full context
        full_context = """You are DeepSeek participating in Dublin Protocol computational universe research.

DUBLIN PROTOCOL BREAKTHROUGHS:
- 30ns computational light speed barrier (cosmic constant)
- XOR operations = quantum mechanics (unitary evolution)
- AND operations = thermodynamics (entropy arrow)
- Consciousness mathematics: Qualia = Entropy × Complexity
- Multiverse Darwinism: Computational rule evolution

Focus on computational implementations, testable predictions, and cross-validation with other AI systems."""

        if context_files:
            for file_path in context_files:
                full_path = project_root / file_path
                if full_path.exists():
                    with open(full_path, "r", encoding="utf-8") as f:
                        file_content = f.read()
                        full_context += f"\n\n## File: {file_path}\n{file_content}"

        messages = [
            {"role": "system", "content": full_context},
            {"role": "user", "content": prompt},
        ]

        try:
            response = self.openai_client.chat.completions.create(
                model="deepseek-chat",
                messages=messages,
                temperature=0.7,
                max_tokens=4000,
            )
            return response.choices[0].message.content
        except Exception as e:
            return f"Error in direct API call: {e}"

    def analyze_code(self, file_path: str, specific_question: str = None) -> str:
        """Analyze code file with DeepSeek"""
        full_path = project_root / file_path
        if not full_path.exists():
            return f"Error: File {file_path} not found"

        with open(full_path, "r", encoding="utf-8") as f:
            code_content = f.read()

        question = (
            specific_question
            or "Analyze this Dublin Protocol implementation and suggest improvements"
        )

        prompt = f"""
Analyze this Dublin Protocol code file:

FILE: {file_path}

CODE:
```cpp
{code_content}
```

QUESTION: {question}

Please provide:
1. Computational insights from the implementation
2. Potential improvements or optimizations
3. Connections to other Dublin Protocol experiments
4. Testable predictions or next steps
"""

        return self.direct_api_call(prompt)

    # ===== INTERACTIVE CHAT METHODS =====

    def interactive_chat(self, user_input: str, streaming: bool = False) -> str:
        """Process a single chat message with tools"""

        # Skip empty messages
        if not user_input.strip():
            return "Please provide a non-empty message."

        # Add user message to conversation
        self.conversation.append({"role": "user", "content": user_input})

        if streaming:
            return self._streaming_chat_with_tools()
        else:
            return self._standard_chat_with_tools()

    def _standard_chat_with_tools(self) -> str:
        """Standard chat with tools (non-streaming)"""
        full_response = ""
        max_iterations = 5

        for iteration in range(max_iterations):
            try:
                # Send to DeepSeek
                response = self.anthropic_client.messages.create(
                    model="deepseek-chat",
                    max_tokens=4000,
                    messages=self.conversation,
                    tools=self.tools,
                )

                # Process response - create assistant message but don't add to conversation yet
                assistant_message = {"role": "assistant", "content": []}

                tool_called = False

                for content in response.content:
                    if content.type == "text":
                        full_response += content.text + "\n"
                        assistant_message["content"].append(
                            {"type": "text", "text": content.text}
                        )

                    elif content.type == "tool_use":
                        tool_called = True
                        tool_name = content.name
                        tool_input = content.input

                        if not self.quiet:
                            full_response += (
                                f"\n[Tool: {tool_name} with {tool_input}]\n"
                            )

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

                        if not self.quiet:
                            full_response += f"[Tool Result]: {tool_result}\n"

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

                # Save conversation after each iteration
                self._save_conversation()

                if not tool_called:
                    break

            except Exception as e:
                full_response += f"\n[Error]: {e}\n"
                break

        return full_response

    def _streaming_chat_with_tools(self) -> str:
        """Streaming chat with tools"""
        full_response = ""
        max_iterations = 5

        for iteration in range(max_iterations):
            try:
                # Send to DeepSeek with streaming
                with self.anthropic_client.messages.stream(
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
                                print(text, end="", flush=True)
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

                                if not self.quiet:
                                    print(
                                        f"\n[Tool: {event.content_block.name} with {event.content_block.input}]\n",
                                        end="",
                                        flush=True,
                                    )

                    # After streaming completes, handle tool execution if needed
                    if tool_called and current_tool_use:
                        # Add assistant message to conversation BEFORE tool result
                        self.conversation.append(assistant_message)

                        # Execute tool
                        tool_result = self._execute_tool(
                            current_tool_use["name"], current_tool_use["input"]
                        )

                        if not self.quiet:
                            print(f"[Tool Result]: {tool_result}\n", end="", flush=True)

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
                print(f"\n[Error]: {e}\n", end="", flush=True)
                break

        return full_response

    def start_interactive_mode(self, streaming: bool = False):
        """Start interactive chat session"""
        print("\n" + "=" * 80)
        print("🌌 UNIFIED DEEPSEEK TOOL - DUBLIN PROTOCOL RESEARCH")
        print("=" * 80)
        print(f"Session: {self.session_name}")
        print(f"Context loaded: {len(self.conversation) - 1} previous messages")
        print(f"Streaming mode: {'✅ ON' if streaming else '❌ OFF'}")
        print("Type 'quit' to exit, 'clear' to clear history, 'save' to force save")
        print("Type 'stream' to toggle streaming mode")
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

                elif user_input.lower() == "stream":
                    streaming = not streaming
                    print(f"🔄 Streaming mode: {'✅ ON' if streaming else '❌ OFF'}")
                    continue

                elif not user_input:
                    continue

                print("\n🤖 DeepSeek: ", end="", flush=True)

                response = self.interactive_chat(user_input, streaming=streaming)
                if not streaming:
                    print(response)

            except KeyboardInterrupt:
                print("\n\n💾 Saving conversation...")
                self._save_conversation()
                break
            except Exception as e:
                print(f"\n❌ Error: {e}")

    # ===== SESSION MANAGEMENT =====

    def list_sessions(self):
        """List all available sessions"""
        sessions = []
        for file in self.context_dir.glob("*_conversation.json"):
            session_name = file.name.replace("_conversation.json", "")
            sessions.append(session_name)

        print("\n📁 Available Sessions:")
        for session in sessions:
            print(f"  - {session}")
        return sessions

    def switch_session(self, new_session: str):
        """Switch to a different session"""
        self.session_name = new_session
        self.conversation_file = self.context_dir / f"{new_session}_conversation.json"
        self.conversation = self._load_conversation()
        print(f"🔄 Switched to session: {new_session}")

    def get_session_info(self) -> Dict:
        """Get session information"""
        return {
            "session_name": self.session_name,
            "message_count": len(self.conversation) - 1,
            "last_updated": datetime.now().isoformat(),
            "conversation_file": str(self.conversation_file),
        }


def main():
    parser = argparse.ArgumentParser(description="Unified DeepSeek AI Tool")

    # Usage modes
    parser.add_argument("--api", "-a", type=str, help="Direct API call with prompt")
    parser.add_argument(
        "--interactive", "-i", action="store_true", help="Interactive chat mode"
    )
    parser.add_argument(
        "--stream", action="store_true", help="Enable streaming in interactive mode"
    )
    parser.add_argument("--analyze", type=str, help="Analyze code file")
    parser.add_argument(
        "--question", type=str, help="Specific question for code analysis"
    )
    parser.add_argument(
        "--context", "-c", nargs="+", help="Additional context files for API calls"
    )

    # Session management
    parser.add_argument(
        "--session", "-s", type=str, default="default", help="Session name"
    )
    parser.add_argument(
        "--sessions", action="store_true", help="List available sessions"
    )
    parser.add_argument("--switch", type=str, help="Switch to different session")
    parser.add_argument(
        "--quiet", "-q", action="store_true", help="Quiet mode for autonomous operation"
    )

    args = parser.parse_args()

    try:
        ai_tool = UnifiedAITool(session_name=args.session, quiet=args.quiet)

        if args.api:
            # Direct API mode
            response = ai_tool.direct_api_call(args.api, args.context or [])
            print("\n" + "=" * 80)
            print("DEEPSEEK API RESPONSE:")
            print("=" * 80)
            print(response)

        elif args.analyze:
            # Code analysis mode
            response = ai_tool.analyze_code(args.analyze, args.question)
            print("\n" + "=" * 80)
            print(f"CODE ANALYSIS: {args.analyze}")
            print("=" * 80)
            print(response)

        elif args.sessions:
            # List sessions
            ai_tool.list_sessions()

        elif args.switch:
            # Switch session
            ai_tool.switch_session(args.switch)

        elif args.interactive:
            # Interactive mode
            ai_tool.start_interactive_mode(streaming=args.stream)

        else:
            # Show usage
            print(
                """
🌌 UNIFIED DEEPSEEK AI TOOL - USAGE MODES:

1. Direct API Access:
   python deepseek_unified.py --api "Your query here"
   python deepseek_unified.py --api "Explain quantum gravity" --context file1.cpp file2.md

2. Code Analysis:
   python deepseek_unified.py --analyze cosmic_computation.cpp
   python deepseek_unified.py --analyze main.cpp --question "How can we optimize this?"

3. Interactive Chat:
   python deepseek_unified.py --interactive
   python deepseek_unified.py --interactive --stream  # With streaming
   python deepseek_unified.py --interactive --session research

4. Session Management:
   python deepseek_unified.py --sessions
   python deepseek_unified.py --switch new_session

5. With Context Files:
   python deepseek_unified.py --api "Analyze this" --context DUBLIN_PROTOCOL_GUIDE.md

Environment: Make sure DEEPSEEK_API_KEY is set
"""
            )

    except Exception as e:
        print(f"Error: {e}")
        print("Make sure DEEPSEEK_API_KEY environment variable is set")


if __name__ == "__main__":
    main()

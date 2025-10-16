#!/usr/bin/env python3
"""
DeepSeek Working Chat - Uses OpenAI-compatible API

Simple working version that uses DeepSeek's actual API endpoint.
"""

import os
import sys
import json
from pathlib import Path
from typing import Dict, List, Optional

project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

try:
    from openai import OpenAI
except ImportError:
    import subprocess
    subprocess.run([sys.executable, "-m", "pip", "install", "openai"])
    from openai import OpenAI


class DeepSeekWorking:
    """Working DeepSeek chat using OpenAI-compatible API"""

    def __init__(self, api_key: Optional[str] = None):
        self.api_key = api_key or os.getenv('DEEPSEEK_API_KEY')
        if not self.api_key:
            raise ValueError("DEEPSEEK_API_KEY environment variable not set")

        # Use OpenAI client with DeepSeek's actual endpoint
        self.client = OpenAI(
            api_key=self.api_key,
            base_url="https://api.deepseek.com"
        )

        self.context_dir = project_root / "tools" / "chat_context"
        self.context_dir.mkdir(exist_ok=True)

    def chat(self, message: str, context: List[Dict] = None) -> str:
        """Simple chat with DeepSeek"""

        # Build messages with Dublin Protocol context
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

Focus on computational implementations, testable predictions, and cross-validation with other AI systems."""
            }
        ]

        # Add conversation context if provided
        if context:
            messages.extend(context)

        # Add current message
        messages.append({"role": "user", "content": message})

        try:
            response = self.client.chat.completions.create(
                model="deepseek-chat",
                messages=messages,
                temperature=0.7,
                max_tokens=4000
            )

            return response.choices[0].message.content

        except Exception as e:
            return f"Error: {e}"

    def interactive_chat(self):
        """Simple interactive chat"""
        print("\n" + "="*80)
        print("🌌 DEEPSEEK WORKING CHAT - DUBLIN PROTOCOL")
        print("="*80)
        print("Type 'quit' to exit")
        print("="*80)

        conversation_history = []

        while True:
            try:
                user_input = input("\n🤔 You: ").strip()

                if user_input.lower() in ['quit', 'exit', 'q']:
                    print("\n👋 Goodbye!")
                    break

                if not user_input:
                    continue

                print("\n🤖 DeepSeek: ", end="", flush=True)

                response = self.chat(user_input, conversation_history)
                print(response)

                # Save to conversation history
                conversation_history.extend([
                    {"role": "user", "content": user_input},
                    {"role": "assistant", "content": response}
                ])

            except KeyboardInterrupt:
                print("\n\n👋 Goodbye!")
                break
            except Exception as e:
                print(f"\n❌ Error: {e}")


def main():
    import argparse

    parser = argparse.ArgumentParser(description='DeepSeek Working Chat')
    parser.add_argument('--message', '-m', type=str, help='Single message')

    args = parser.parse_args()

    try:
        deepseek = DeepSeekWorking()

        if args.message:
            # Single message mode
            response = deepseek.chat(args.message)
            print("\n" + "="*80)
            print("DEEPSEEK RESPONSE:")
            print("="*80)
            print(response)
        else:
            # Interactive mode
            deepseek.interactive_chat()

    except Exception as e:
        print(f"Error: {e}")
        print("Make sure DEEPSEEK_API_KEY is set correctly")


if __name__ == "__main__":
    main()
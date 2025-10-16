#!/usr/bin/env python3
"""
Unified AI Tool Framework

Extensible framework for multiple AI APIs:
- DeepSeek (primary)
- OpenAI (GPT-4, etc.)
- Anthropic (Claude)
- Google (Gemini)
- Custom APIs

Designed for Dublin Protocol multi-AI collaboration research.
"""

import os
import sys
import json
import argparse
from pathlib import Path
from typing import Dict, List, Optional, Any
from abc import ABC, abstractmethod

project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))


class AIProvider(ABC):
    """Abstract base class for AI providers"""

    @abstractmethod
    def chat_completion(self, messages: List[Dict], **kwargs) -> str:
        """Send chat completion request"""
        pass

    @abstractmethod
    def get_available_models(self) -> List[str]:
        """Get list of available models"""
        pass


class DeepSeekProvider(AIProvider):
    """DeepSeek API provider"""

    def __init__(self, api_key: str):
        try:
            from openai import OpenAI
            self.client = OpenAI(
                api_key=api_key,
                base_url="https://api.deepseek.com"
            )
        except ImportError:
            raise ImportError("OpenAI package required for DeepSeek provider")

    def chat_completion(self, messages: List[Dict], **kwargs) -> str:
        try:
            response = self.client.chat.completions.create(
                model=kwargs.get('model', 'deepseek-chat'),
                messages=messages,
                temperature=kwargs.get('temperature', 0.7),
                max_tokens=kwargs.get('max_tokens', 4000)
            )
            return response.choices[0].message.content
        except Exception as e:
            return f"DeepSeek Error: {e}"

    def get_available_models(self) -> List[str]:
        return ["deepseek-chat", "deepseek-coder"]


class OpenAIProvider(AIProvider):
    """OpenAI API provider"""

    def __init__(self, api_key: str):
        try:
            from openai import OpenAI
            self.client = OpenAI(api_key=api_key)
        except ImportError:
            raise ImportError("OpenAI package required for OpenAI provider")

    def chat_completion(self, messages: List[Dict], **kwargs) -> str:
        try:
            response = self.client.chat.completions.create(
                model=kwargs.get('model', 'gpt-4'),
                messages=messages,
                temperature=kwargs.get('temperature', 0.7),
                max_tokens=kwargs.get('max_tokens', 4000)
            )
            return response.choices[0].message.content
        except Exception as e:
            return f"OpenAI Error: {e}"

    def get_available_models(self) -> List[str]:
        return ["gpt-4", "gpt-4-turbo", "gpt-3.5-turbo"]


class AnthropicProvider(AIProvider):
    """Anthropic Claude API provider"""

    def __init__(self, api_key: str):
        try:
            import anthropic
            self.client = anthropic.Anthropic(api_key=api_key)
        except ImportError:
            raise ImportError("Anthropic package required for Claude provider")

    def chat_completion(self, messages: List[Dict], **kwargs) -> str:
        try:
            # Convert messages to Anthropic format
            anthropic_messages = []
            for msg in messages:
                if msg["role"] == "system":
                    # Anthropic uses first user message for system content
                    if not anthropic_messages:
                        anthropic_messages.append({
                            "role": "user",
                            "content": msg["content"]
                        })
                    else:
                        # Add system content to first user message
                        anthropic_messages[0]["content"] = msg["content"] + "\n\n" + anthropic_messages[0]["content"]
                else:
                    anthropic_messages.append({
                        "role": msg["role"],
                        "content": msg["content"]
                    })

            response = self.client.messages.create(
                model=kwargs.get('model', 'claude-3-sonnet-20240229'),
                max_tokens=kwargs.get('max_tokens', 4000),
                messages=anthropic_messages
            )
            return response.content[0].text
        except Exception as e:
            return f"Anthropic Error: {e}"

    def get_available_models(self) -> List[str]:
        return ["claude-3-opus-20240229", "claude-3-sonnet-20240229", "claude-3-haiku-20240307"]


class UnifiedAIFramework:
    """Unified framework for multiple AI providers"""

    def __init__(self):
        self.providers = {}
        self.context_dir = project_root / "tools" / "ai_context"
        self.context_dir.mkdir(exist_ok=True)

    def register_provider(self, name: str, provider: AIProvider):
        """Register an AI provider"""
        self.providers[name] = provider

    def get_provider(self, name: str) -> Optional[AIProvider]:
        """Get a registered provider"""
        return self.providers.get(name)

    def list_providers(self) -> List[str]:
        """List all registered providers"""
        return list(self.providers.keys())

    def query_provider(self, provider_name: str, prompt: str, context: str = None, **kwargs) -> str:
        """Query a specific provider"""
        provider = self.get_provider(provider_name)
        if not provider:
            return f"Provider '{provider_name}' not found. Available: {', '.join(self.list_providers())}"

        # Build messages with Dublin Protocol context
        messages = []

        if context:
            messages.append({
                "role": "system",
                "content": context
            })
        else:
            # Default Dublin Protocol context
            messages.append({
                "role": "system",
                "content": """You are participating in Dublin Protocol computational universe research.

DUBLIN PROTOCOL BREAKTHROUGHS:
- 30ns computational light speed barrier (cosmic constant)
- XOR operations = quantum mechanics (unitary evolution)
- AND operations = thermodynamics (entropy arrow)
- Consciousness mathematics: Qualia = Entropy × Complexity
- Multiverse Darwinism: Computational rule evolution

Focus on computational implementations, testable predictions, and cross-validation."""
            })

        messages.append({
            "role": "user",
            "content": prompt
        })

        return provider.chat_completion(messages, **kwargs)

    def cross_validate(self, prompt: str, providers: List[str] = None, context: str = None) -> Dict[str, str]:
        """Cross-validate a query across multiple providers"""
        if providers is None:
            providers = self.list_providers()

        results = {}
        for provider_name in providers:
            print(f"🤖 Querying {provider_name}...")
            results[provider_name] = self.query_provider(provider_name, prompt, context)

        return results

    def save_response(self, provider_name: str, prompt: str, response: str, filename: str = None):
        """Save AI response to file"""
        if filename is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"{provider_name}_{timestamp}.json"

        file_path = self.context_dir / filename
        data = {
            "provider": provider_name,
            "prompt": prompt,
            "response": response,
            "timestamp": datetime.now().isoformat()
        }

        with open(file_path, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2, ensure_ascii=False)

        return str(file_path)


# Factory function to create framework with common providers
def create_ai_framework() -> UnifiedAIFramework:
    """Create AI framework with available providers"""
    framework = UnifiedAIFramework()

    # Register DeepSeek if API key available
    deepseek_key = os.getenv('DEEPSEEK_API_KEY')
    if deepseek_key:
        try:
            framework.register_provider("deepseek", DeepSeekProvider(deepseek_key))
        except Exception as e:
            print(f"Warning: Could not register DeepSeek: {e}")

    # Register OpenAI if API key available
    openai_key = os.getenv('OPENAI_API_KEY')
    if openai_key:
        try:
            framework.register_provider("openai", OpenAIProvider(openai_key))
        except Exception as e:
            print(f"Warning: Could not register OpenAI: {e}")

    # Register Anthropic if API key available
    anthropic_key = os.getenv('ANTHROPIC_API_KEY')
    if anthropic_key:
        try:
            framework.register_provider("anthropic", AnthropicProvider(anthropic_key))
        except Exception as e:
            print(f"Warning: Could not register Anthropic: {e}")

    return framework


def main():
    """Command-line interface for unified AI framework"""
    parser = argparse.ArgumentParser(description='Unified AI Framework')

    # Query options
    parser.add_argument('--query', '-q', type=str, help='Query to send to AI')
    parser.add_argument('--provider', '-p', type=str, default='deepseek',
                       help='AI provider (deepseek, openai, anthropic)')
    parser.add_argument('--model', type=str, help='Specific model to use')
    parser.add_argument('--context', '-c', type=str, help='Custom context/prompt')

    # Multi-provider options
    parser.add_argument('--cross-validate', '-x', action='store_true',
                       help='Cross-validate across all available providers')
    parser.add_argument('--providers', nargs='+', help='Specific providers for cross-validation')

    # Framework management
    parser.add_argument('--list-providers', action='store_true', help='List available providers')
    parser.add_argument('--list-models', type=str, help='List models for a provider')

    args = parser.parse_args()

    framework = create_ai_framework()

    if args.list_providers:
        print("\n🤖 Available AI Providers:")
        for provider in framework.list_providers():
            print(f"  - {provider}")
        return

    if args.list_models:
        provider = framework.get_provider(args.list_models)
        if provider:
            print(f"\n🤖 Available models for {args.list_models}:")
            for model in provider.get_available_models():
                print(f"  - {model}")
        else:
            print(f"Provider '{args.list_models}' not found")
        return

    if args.query:
        if args.cross_validate:
            print("\n🔬 Cross-Validating Query...")
            providers = args.providers if args.providers else None
            results = framework.cross_validate(args.query, providers, args.context)

            print("\n" + "="*80)
            print("CROSS-VALIDATION RESULTS")
            print("="*80)
            for provider, response in results.items():
                print(f"\n🌌 {provider.upper()}:")
                print("-" * 40)
                print(response)

                # Save each response
                filename = framework.save_response(provider, args.query, response)
                print(f"💾 Saved to: {filename}")

        else:
            # Single provider query
            kwargs = {}
            if args.model:
                kwargs['model'] = args.model

            response = framework.query_provider(args.provider, args.query, args.context, **kwargs)

            print("\n" + "="*80)
            print(f"{args.provider.upper()} RESPONSE:")
            print("="*80)
            print(response)

            # Save response
            filename = framework.save_response(args.provider, args.query, response)
            print(f"\n💾 Saved to: {filename}")

    else:
        # Show usage
        print("""
🌌 UNIFIED AI FRAMEWORK - DUBLIN PROTOCOL RESEARCH

USAGE MODES:

1. Single Provider Query:
   python ai_unified.py --query "Explain quantum gravity" --provider deepseek
   python ai_unified.py --query "Analyze code" --provider openai --model gpt-4

2. Cross-Validation:
   python ai_unified.py --query "30ns barrier" --cross-validate
   python ai_unified.py --query "Consciousness math" --cross-validate --providers deepseek openai

3. Framework Management:
   python ai_unified.py --list-providers
   python ai_unified.py --list-models deepseek

4. With Custom Context:
   python ai_unified.py --query "Test prediction" --context "Focus on computational implementations"

ENVIRONMENT VARIABLES:
- DEEPSEEK_API_KEY: For DeepSeek access
- OPENAI_API_KEY: For OpenAI access
- ANTHROPIC_API_KEY: For Claude access
""")


if __name__ == "__main__":
    from datetime import datetime
    main()
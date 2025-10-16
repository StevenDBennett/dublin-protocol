#!/usr/bin/env python3
"""
Dublin Protocol Session Manager

Manage and view your DeepSeek chat sessions for continuous research context.
"""

import json
import argparse
from pathlib import Path
from datetime import datetime

project_root = Path(__file__).parent.parent
context_dir = project_root / "tools" / "chat_context"


def list_sessions():
    """List all available chat sessions"""
    print("\n" + "="*80)
    print("📚 DUBLIN PROTOCOL CHAT SESSIONS")
    print("="*80)

    sessions = []
    for file in context_dir.glob("*_conversation.json"):
        session_name = file.name.replace("_conversation.json", "")

        try:
            with open(file, 'r', encoding='utf-8') as f:
                data = json.load(f)

            message_count = len(data) - 1  # Exclude system message

            # Find last user message timestamp
            last_message = ""
            for msg in reversed(data):
                if msg.get("role") == "user" and "content" in msg:
                    if isinstance(msg["content"], str):
                        last_message = msg["content"][:100] + "..." if len(msg["content"]) > 100 else msg["content"]
                    break

            sessions.append({
                "name": session_name,
                "messages": message_count,
                "last_message": last_message,
                "file": file
            })

        except Exception as e:
            print(f"Error reading {file}: {e}")

    if not sessions:
        print("No chat sessions found.")
        print("Start a session: python tools/deepseek_interactive.py")
        return

    for i, session in enumerate(sessions, 1):
        print(f"\n{i}. {session['name']}")
        print(f"   Messages: {session['messages']}")
        if session['last_message']:
            print(f"   Last: {session['last_message']}")

    return sessions


def show_session(session_name: str, limit: int = 10):
    """Show recent messages from a session"""
    file_path = context_dir / f"{session_name}_conversation.json"

    if not file_path.exists():
        print(f"Session '{session_name}' not found.")
        return

    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            data = json.load(f)

        print(f"\n" + "="*80)
        print(f"💬 SESSION: {session_name}")
        print("="*80)
        print(f"Total messages: {len(data) - 1}")

        # Show recent messages (skip system message)
        recent_messages = data[1:][-limit:] if len(data) > 1 else []

        for msg in recent_messages:
            role = msg.get("role", "unknown")
            content = msg.get("content", "")

            if role == "user":
                print(f"\n👤 YOU: {content}")
            elif role == "assistant":
                # Handle tool calls in assistant messages
                if isinstance(content, list):
                    for item in content:
                        if item.get("type") == "text":
                            print(f"\n🤖 DEEPSEEK: {item.get('text', '')}")
                else:
                    print(f"\n🤖 DEEPSEEK: {content}")

    except Exception as e:
        print(f"Error reading session: {e}")


def delete_session(session_name: str):
    """Delete a chat session"""
    file_path = context_dir / f"{session_name}_conversation.json"

    if not file_path.exists():
        print(f"Session '{session_name}' not found.")
        return

    try:
        file_path.unlink()
        print(f"✅ Deleted session: {session_name}")
    except Exception as e:
        print(f"Error deleting session: {e}")


def main():
    parser = argparse.ArgumentParser(description='Dublin Protocol Session Manager')
    parser.add_argument('--list', '-l', action='store_true', help='List all sessions')
    parser.add_argument('--show', '-s', type=str, help='Show specific session')
    parser.add_argument('--delete', '-d', type=str, help='Delete specific session')
    parser.add_argument('--limit', type=int, default=10, help='Number of messages to show')

    args = parser.parse_args()

    if args.list:
        list_sessions()
    elif args.show:
        show_session(args.show, args.limit)
    elif args.delete:
        delete_session(args.delete)
    else:
        # Default: list sessions
        list_sessions()
        print("\n" + "="*80)
        print("Usage:")
        print("  python session_manager.py --list          # List all sessions")
        print("  python session_manager.py --show session  # Show session messages")
        print("  python session_manager.py --delete session # Delete session")


if __name__ == "__main__":
    main()
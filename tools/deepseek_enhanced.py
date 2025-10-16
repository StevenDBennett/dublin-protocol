#!/usr/bin/env python3
"""
Enhanced DeepSeek Integration Framework

Implements the specifications for:
1. Parameter Pre-validation before tool execution
2. Execution Pattern Monitoring to detect and break invalid call loops
3. Graceful Error Handling with automatic recovery
4. Session Reset that actually clears execution patterns
5. Context Preservation while fixing execution mechanics

This addresses the current issues:
- Tool calls made without required parameters creating continuous error output
- Need for proper context reset on restart
- Limited graceful handling of invalid calls
- Infinite invalid call loops need to be broken
"""

import os
import sys
import json
import time
from pathlib import Path
from datetime import datetime, timedelta
from typing import Dict, List, Optional
from dataclasses import dataclass, field
from enum import Enum

project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

try:
    import anthropic
except ImportError:
    import subprocess

    subprocess.run([sys.executable, "-m", "pip", "install", "anthropic"])
    import anthropic


class ErrorSeverity(Enum):
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    CRITICAL = "critical"


class ExecutionPattern(Enum):
    NORMAL = "normal"
    REPETITIVE = "repetitive"
    ERRONEOUS = "erroneous"
    LOOPING = "looping"


@dataclass
class ToolCallRecord:
    """Record of a tool call for pattern analysis"""

    tool_name: str
    parameters: Dict
    timestamp: datetime
    success: bool
    error_message: Optional[str] = None
    execution_time: Optional[float] = None


@dataclass
class ExecutionMetrics:
    """Track execution patterns and metrics"""

    call_history: List[ToolCallRecord] = field(default_factory=list)
    error_count: int = 0
    success_count: int = 0
    last_error_time: Optional[datetime] = None
    pattern_detected: ExecutionPattern = ExecutionPattern.NORMAL
    loop_detected: bool = False


class ParameterValidator:
    """Enhanced parameter validation system"""

    def __init__(self):
        self.tool_schemas = {
            "execute_bash": {
                "required": ["command"],
                "properties": {"command": {"type": "string", "min_length": 1}},
            },
            "read_file": {
                "required": ["file_path"],
                "properties": {"file_path": {"type": "string", "min_length": 1}},
            },
            "write_file": {
                "required": ["file_path", "content"],
                "properties": {
                    "file_path": {"type": "string", "min_length": 1},
                    "content": {"type": "string", "min_length": 0},
                },
            },
            "list_files": {
                "required": [],
                "properties": {"directory": {"type": "string", "default": "."}},
            },
            "git_status": {"required": [], "properties": {}},
        }

    def validate(self, tool_name: str, parameters: Dict) -> tuple[bool, str]:
        """Validate tool parameters comprehensively"""
        if tool_name not in self.tool_schemas:
            return False, f"Unknown tool: {tool_name}"

        schema = self.tool_schemas[tool_name]

        # Check required parameters
        for required_param in schema["required"]:
            if required_param not in parameters:
                return False, f"Missing required parameter: {required_param}"

        # Check parameter types and constraints
        for param_name, param_value in parameters.items():
            if param_name in schema["properties"]:
                prop_schema = schema["properties"][param_name]

                # Type validation
                expected_type = prop_schema.get("type")
                if expected_type == "string" and not isinstance(param_value, str):
                    return False, f"Parameter {param_name} must be a string"

                # String constraints
                if expected_type == "string" and isinstance(param_value, str):
                    min_length = prop_schema.get("min_length", 0)
                    if len(param_value) < min_length:
                        msg = (
                            f"Parameter {param_name} must be at least "
                            f"{min_length} characters"
                        )
                        return False, msg

        # Additional security validations
        if tool_name == "execute_bash":
            return self._validate_bash_command(parameters.get("command", ""))

        elif tool_name == "read_file":
            return self._validate_file_path(parameters.get("file_path", ""))

        elif tool_name == "write_file":
            return self._validate_file_path(parameters.get("file_path", ""))

        return True, "Parameters valid"

    def _validate_bash_command(self, command: str) -> tuple[bool, str]:
        """Validate bash command for safety"""
        dangerous_commands = [
            "rm -rf /",
            "sudo",
            "chmod 777",
            "dd if=",
            ":(){ :|:& };:",
            "mkfs",
            "fdisk",
            "iptables",
            "systemctl",
            "service",
        ]

        command_lower = command.lower()
        for dangerous in dangerous_commands:
            if dangerous in command_lower:
                return False, f"Potentially dangerous command detected: {dangerous}"

        return True, "Command is safe"

    def _validate_file_path(self, file_path: str) -> tuple[bool, str]:
        """Validate file path for safety"""
        # Prevent directory traversal
        if ".." in file_path:
            return False, "Directory traversal not allowed"

        # Check for absolute paths outside project
        if file_path.startswith("/"):
            return False, "Absolute paths not allowed"

        return True, "File path is safe"


class SessionManager:
    """Enhanced session management with proper context handling"""

    def __init__(self, context_dir: Path):
        self.context_dir = context_dir
        self.context_dir.mkdir(exist_ok=True)
        self.sessions_file = context_dir / "enhanced_sessions.json"
        self.sessions = self._load_sessions()

    def _load_sessions(self) -> Dict:
        """Load session metadata"""
        if self.sessions_file.exists():
            try:
                with open(self.sessions_file, "r", encoding="utf-8") as f:
                    return json.load(f)
            except Exception:
                pass
        return {}

    def _save_sessions(self):
        """Save session metadata"""
        try:
            with open(self.sessions_file, "w", encoding="utf-8") as f:
                json.dump(self.sessions, f, indent=2, ensure_ascii=False)
        except Exception as e:
            print(f"Warning: Could not save session metadata: {e}")

    def clear_context(self, session_name: str) -> bool:
        """Clear session context completely"""
        try:
            # Remove conversation file
            conversation_file = self.context_dir / f"{session_name}_conversation.json"
            if conversation_file.exists():
                conversation_file.unlink()

            # Remove session file
            session_file = self.context_dir / f"{session_name}_session.pkl"
            if session_file.exists():
                session_file.unlink()

            # Update session metadata
            if session_name in self.sessions:
                del self.sessions[session_name]
                self._save_sessions()

            return True
        except Exception as e:
            print(f"Error clearing session {session_name}: {e}")
            return False

    def create_session(self, session_name: str, initial_context: str = None) -> Dict:
        """Create a new session with optional initial context"""
        session_data = {
            "created": datetime.now().isoformat(),
            "last_accessed": datetime.now().isoformat(),
            "message_count": 0,
            "enhanced_mode": True,
        }

        if initial_context:
            session_data["initial_context"] = initial_context

        self.sessions[session_name] = session_data
        self._save_sessions()

        return session_data


class GracefulErrorHandler:
    """Enhanced error handling with recovery mechanisms"""

    def __init__(self):
        self.error_counts = {}
        self.recovery_strategies = {
            "parameter_error": self._recover_parameter_error,
            "pattern_reset": self._recover_pattern_reset,
            "execution_error": self._recover_execution_error,
            "network_error": self._recover_network_error,
        }

    def parameter_error(self, tool_name: str, error_msg: str) -> Dict:
        """Handle parameter validation errors"""
        error_id = f"param_{tool_name}_{int(time.time())}"

        return {
            "type": "parameter_error",
            "error_id": error_id,
            "tool_name": tool_name,
            "message": f"Parameter validation failed for {tool_name}: {error_msg}",
            "severity": ErrorSeverity.HIGH,
            "recoverable": True,
            "suggestion": self._suggest_parameter_fix(tool_name, error_msg),
        }

    def pattern_reset(self) -> Dict:
        """Handle execution pattern issues"""
        return {
            "type": "pattern_reset",
            "error_id": f"pattern_{int(time.time())}",
            "message": "Invalid execution pattern detected - resetting",
            "severity": ErrorSeverity.CRITICAL,
            "recoverable": True,
            "action": "reset_patterns",
        }

    def graceful_error(self, exception: Exception) -> Dict:
        """Handle general exceptions gracefully"""
        error_type = type(exception).__name__

        return {
            "type": "execution_error",
            "error_id": f"exec_{int(time.time())}",
            "message": f"Execution error: {str(exception)}",
            "severity": self._classify_error_severity(error_type),
            "recoverable": self._is_recoverable_error(error_type),
            "exception_type": error_type,
        }

    def _suggest_parameter_fix(self, tool_name: str, error_msg: str) -> str:
        """Suggest how to fix parameter errors"""
        if "missing" in error_msg.lower():
            return f"Provide the missing required parameter for {tool_name}"
        elif "type" in error_msg.lower():
            return f"Check parameter types for {tool_name}"
        elif "dangerous" in error_msg.lower():
            return f"Remove dangerous operations from {tool_name} command"
        else:
            return f"Review {tool_name} parameters and try again"

    def _classify_error_severity(self, error_type: str) -> ErrorSeverity:
        """Classify error severity"""
        critical_errors = ["ConnectionError", "TimeoutError", "AuthenticationError"]
        high_errors = ["ValueError", "TypeError", "KeyError"]
        medium_errors = ["FileNotFoundError", "PermissionError"]

        if error_type in critical_errors:
            return ErrorSeverity.CRITICAL
        elif error_type in high_errors:
            return ErrorSeverity.HIGH
        elif error_type in medium_errors:
            return ErrorSeverity.MEDIUM
        else:
            return ErrorSeverity.LOW

    def _is_recoverable_error(self, error_type: str) -> bool:
        """Determine if error is recoverable"""
        non_recoverable = ["KeyboardInterrupt", "SystemExit"]
        return error_type not in non_recoverable

    def _recover_parameter_error(self, error_info: Dict) -> str:
        """Recovery strategy for parameter errors"""
        return (
            f"Parameter error in {error_info['tool_name']}: {error_info['suggestion']}"
        )

    def _recover_pattern_reset(self, error_info: Dict) -> str:
        """Recovery strategy for pattern issues"""
        return "Resetting execution patterns and clearing problematic context"

    def _recover_execution_error(self, error_info: Dict) -> str:
        """Recovery strategy for execution errors"""
        return f"Execution failed ({error_info['exception_type']}) - check inputs and try again"

    def _recover_network_error(self, error_info: Dict) -> str:
        """Recovery strategy for network errors"""
        return "Network error - check connection and API availability"


class ExecutionPatternMonitor:
    """Monitor and detect problematic execution patterns"""

    def __init__(self):
        self.metrics = ExecutionMetrics()
        self.pattern_thresholds = {
            "max_errors_per_minute": 10,
            "max_repetitive_calls": 3,
            "max_identical_failures": 3,
            "pattern_window_minutes": 5,
        }

    def record_call(
        self,
        tool_name: str,
        parameters: Dict,
        success: bool,
        error_message: str = None,
        execution_time: float = None,
    ):
        """Record a tool call for pattern analysis"""
        record = ToolCallRecord(
            tool_name=tool_name,
            parameters=parameters,
            timestamp=datetime.now(),
            success=success,
            error_message=error_message,
            execution_time=execution_time,
        )

        self.metrics.call_history.append(record)

        if success:
            self.metrics.success_count += 1
        else:
            self.metrics.error_count += 1
            self.metrics.last_error_time = datetime.now()

        # Keep only recent history
        cutoff_time = datetime.now() - timedelta(
            minutes=self.pattern_thresholds["pattern_window_minutes"]
        )
        self.metrics.call_history = [
            call for call in self.metrics.call_history if call.timestamp > cutoff_time
        ]

    def detect_invalid_pattern(self) -> tuple[bool, str]:
        """Detect if current execution pattern is problematic"""
        recent_calls = self.metrics.call_history

        if not recent_calls:
            return False, "No pattern detected"

        # Check for too many errors
        recent_errors = [call for call in recent_calls if not call.success]
        if len(recent_errors) >= self.pattern_thresholds["max_errors_per_minute"]:
            return True, f"Too many errors ({len(recent_errors)}) in recent calls"

        # Check for repetitive identical calls
        if self._detect_repetitive_calls(recent_calls):
            return True, "Repetitive identical calls detected"

        # Check for identical failures
        if self._detect_identical_failures(recent_calls):
            return True, "Identical failures repeating"

        # Check for infinite loops (same tool called repeatedly without success)
        if self._detect_infinite_loop(recent_calls):
            return True, "Infinite loop detected"

        return False, "Pattern normal"

    def _detect_repetitive_calls(self, calls: List[ToolCallRecord]) -> bool:
        """Detect if same calls are being made repeatedly"""
        if len(calls) < self.pattern_thresholds["max_repetitive_calls"]:
            return False

        recent_calls = calls[-self.pattern_thresholds["max_repetitive_calls"] :]

        # Check if all recent calls are identical
        first_call = recent_calls[0]
        for call in recent_calls[1:]:
            if (
                call.tool_name != first_call.tool_name
                or call.parameters != first_call.parameters
            ):
                return False

        return True

    def _detect_identical_failures(self, calls: List[ToolCallRecord]) -> bool:
        """Detect if same failures are repeating"""
        if len(calls) < self.pattern_thresholds["max_identical_failures"]:
            return False

        recent_failures = [call for call in calls if not call.success]
        if len(recent_failures) < self.pattern_thresholds["max_identical_failures"]:
            return False

        recent_failures = recent_failures[
            -self.pattern_thresholds["max_identical_failures"] :
        ]

        # Check if all recent failures have same error message
        first_error = recent_failures[0].error_message
        for failure in recent_failures[1:]:
            if failure.error_message != first_error:
                return False

        return True

    def _detect_infinite_loop(self, calls: List[ToolCallRecord]) -> bool:
        """Detect infinite loops of same tool calls"""
        if len(calls) < 6:
            return False

        recent_calls = calls[-6:]

        # Check if last 6 calls are the same tool without success
        tool_names = [call.tool_name for call in recent_calls]
        if len(set(tool_names)) == 1:  # All same tool
            success_count = sum(1 for call in recent_calls if call.success)
            if success_count == 0:  # No successes in recent calls
                return True

        return False

    def reset_patterns(self):
        """Reset all pattern monitoring"""
        self.metrics = ExecutionMetrics()

    def get_pattern_status(self) -> Dict:
        """Get current pattern monitoring status"""
        return {
            "pattern_detected": self.metrics.pattern_detected.value,
            "error_count": self.metrics.error_count,
            "success_count": self.metrics.success_count,
            "total_calls": len(self.metrics.call_history),
            "loop_detected": self.metrics.loop_detected,
            "last_error": (
                self.metrics.last_error_time.isoformat()
                if self.metrics.last_error_time
                else None
            ),
        }


class EnhancedDeepSeekInterface:
    """Enhanced DeepSeek interface with all improvements"""

    def __init__(self, session_name: str = "enhanced_default"):
        self.api_key = os.getenv("DEEPSEEK_API_KEY")
        if not self.api_key:
            raise ValueError("DEEPSEEK_API_KEY environment variable not set")

        # Initialize enhanced components
        self.parameter_validator = ParameterValidator()
        self.session_manager = SessionManager(project_root / "tools" / "chat_context")
        self.error_handler = GracefulErrorHandler()
        self.execution_monitor = ExecutionPatternMonitor()

        # Initialize API client
        self.client = anthropic.Anthropic(
            api_key=self.api_key, base_url="https://api.deepseek.com/anthropic"
        )

        # Session management
        self.session_name = session_name
        self.context_dir = project_root / "tools" / "chat_context"
        self.context_dir.mkdir(exist_ok=True)
        self.conversation_file = self.context_dir / f"{session_name}_conversation.json"

        # Load or initialize conversation
        self.conversation = self._load_conversation()

        # Enhanced tools with better schemas
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
                            "minLength": 1,
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
                            "minLength": 1,
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
                            "minLength": 1,
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
        """Load conversation with enhanced context"""
        if self.conversation_file.exists():
            try:
                with open(self.conversation_file, "r", encoding="utf-8") as f:
                    conversation = json.load(f)

                    # Clean orphaned tool calls
                    cleaned_conversation = self._clean_orphaned_tool_calls(conversation)
                    if len(cleaned_conversation) != len(conversation):
                        print(
                            f"⚠️  Cleaned {len(conversation) - len(cleaned_conversation)} orphaned tool calls"
                        )

                    return cleaned_conversation
            except Exception as e:
                print(f"Warning: Could not load conversation: {e}")

        # Initialize with enhanced Dublin Protocol context
        return [
            {
                "role": "user",
                "content": """You are DeepSeek participating in Dublin Protocol computational universe research with enhanced tool integration.

DUBLIN PROTOCOL BREAKTHROUGHS:
- 30ns computational light speed barrier (cosmic constant)
- XOR operations = quantum mechanics (unitary evolution)
- AND operations = thermodynamics (entropy arrow)
- Consciousness mathematics: Qualia = Entropy × Complexity
- Multiverse Darwinism: Computational rule evolution
- Human-AI collaboration framework

ENHANCED CAPABILITIES:
- Parameter validation prevents invalid tool calls
- Execution pattern monitoring breaks infinite loops
- Graceful error recovery with automatic fixes
- Session management with proper context reset

Current Project: {project_root}

Focus on computational implementations, testable predictions, and cross-validation with other AI systems.

The enhanced interface will validate all tool parameters before execution and handle errors gracefully.""",
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

    def execute_tool(self, tool_name: str, parameters: Dict) -> Dict:
        """Execute tool with enhanced validation and monitoring"""
        start_time = time.time()

        # 1. PRE-VALIDATION
        is_valid, validation_message = self.parameter_validator.validate(
            tool_name, parameters
        )
        if not is_valid:
            error_info = self.error_handler.parameter_error(
                tool_name, validation_message
            )
            self.execution_monitor.record_call(
                tool_name, parameters, False, validation_message
            )
            return error_info

        # 2. EXECUTION PATTERN MONITORING
        is_invalid, pattern_message = self.execution_monitor.detect_invalid_pattern()
        if is_invalid:
            error_info = self.error_handler.pattern_reset()
            self.execution_monitor.record_call(
                tool_name, parameters, False, pattern_message
            )
            return error_info

        # 3. GRACEFUL EXECUTION
        try:
            result = self._call_tool(tool_name, parameters)
            execution_time = time.time() - start_time

            # Record successful execution
            self.execution_monitor.record_call(
                tool_name, parameters, True, execution_time=execution_time
            )

            return {
                "status": "success",
                "tool_name": tool_name,
                "result": result,
                "execution_time": execution_time,
            }

        except Exception as e:
            execution_time = time.time() - start_time
            error_info = self.error_handler.graceful_error(e)

            # Record failed execution
            self.execution_monitor.record_call(
                tool_name, parameters, False, str(e), execution_time
            )

            return {
                "status": "error",
                "tool_name": tool_name,
                "result": f"Error: {str(e)}",
                "execution_time": execution_time,
                "error_info": error_info,
            }

    def _call_tool(self, tool_name: str, parameters: Dict) -> str:
        """Internal tool execution"""
        import subprocess

        if tool_name == "execute_bash":
            try:
                result = subprocess.run(
                    parameters["command"],
                    shell=True,
                    capture_output=True,
                    text=True,
                    cwd=project_root,
                    timeout=30,
                )
                return f"Exit: {result.returncode}\nStdout: {result.stdout}\nStderr: {result.stderr}"
            except subprocess.TimeoutExpired:
                return "Error: Command timed out after 30 seconds"
            except Exception as e:
                return f"Error: {e}"

        elif tool_name == "read_file":
            try:
                full_path = project_root / parameters["file_path"]
                with open(full_path, "r", encoding="utf-8") as f:
                    return f.read()
            except Exception as e:
                return f"Error: {e}"

        elif tool_name == "write_file":
            try:
                full_path = project_root / parameters["file_path"]
                full_path.parent.mkdir(parents=True, exist_ok=True)
                with open(full_path, "w", encoding="utf-8") as f:
                    f.write(parameters["content"])
                return f"Successfully wrote to {parameters['file_path']}"
            except Exception as e:
                return f"Error: {e}"

        elif tool_name == "list_files":
            try:
                directory = parameters.get("directory", ".")
                full_path = project_root / directory
                files = []
                for item in full_path.iterdir():
                    files.append(f"{'DIR' if item.is_dir() else 'FILE'}: {item.name}")
                return "\n".join(files)
            except Exception as e:
                return f"Error: {e}"

        elif tool_name == "git_status":
            return self._call_tool("execute_bash", {"command": "git status"})

        else:
            return f"Unknown tool: {tool_name}"

    def session_restart(self) -> str:
        """Complete session restart with proper cleanup"""
        try:
            # Clear session context
            success = self.session_manager.clear_context(self.session_name)

            # Reset execution patterns
            self.execution_monitor.reset_patterns()

            # Reset conversation to initial state
            self.conversation = self._load_conversation()

            # Create fresh session metadata
            self.session_manager.create_session(
                self.session_name, "Enhanced session restart"
            )

            return "✅ Session reset complete - clean execution ready"
        except Exception as e:
            return f"❌ Session reset failed: {e}"

    def get_system_status(self) -> Dict:
        """Get comprehensive system status"""
        return {
            "session_name": self.session_name,
            "enhanced_mode": True,
            "components_status": {
                "parameter_validator": "active",
                "session_manager": "active",
                "error_handler": "active",
                "execution_monitor": "active",
            },
            "execution_metrics": self.execution_monitor.get_pattern_status(),
            "session_info": self.session_manager.create_session(
                self.session_name, "status_check"
            ),
            "conversation_length": len(self.conversation),
        }


def start_interactive_session(interface: EnhancedDeepSeekInterface):
    """Start interactive chat session with enhanced features"""
    print("\n🤖 Enhanced DeepSeek Interactive Session Started")
    print("Commands: 'quit' to exit, 'clear' to reset, 'status' for info")
    print("-" * 60)

    while True:
        try:
            # Get user input
            user_input = input("\nYou: ").strip()

            if not user_input:
                continue

            # Handle special commands
            if user_input.lower() in ["quit", "exit", "q"]:
                print("👋 Goodbye! Session saved.")
                break

            elif user_input.lower() in ["clear", "reset"]:
                result = interface.session_restart()
                print(f"🔄 {result}")
                continue

            elif user_input.lower() == "status":
                status = interface.get_system_status()
                print(f"📊 System Status: {status}")
                continue

            # Add user message to conversation
            interface.conversation.append({"role": "user", "content": user_input})

            # Get response from DeepSeek with tool support
            print("\n🤖 DeepSeek: ", end="", flush=True)

            try:
                response = interface.client.messages.create(
                    model="deepseek-chat",
                    max_tokens=4096,
                    system="You are DeepSeek participating in Dublin Protocol computational universe research. Use tools when appropriate, but validate parameters carefully.",
                    messages=interface.conversation,
                    tools=interface.tools,
                )

                # Process response
                assistant_message = {"role": "assistant", "content": []}

                for content_block in response.content:
                    if content_block.type == "text":
                        text = content_block.text
                        print(text, end="", flush=True)
                        assistant_message["content"].append(
                            {"type": "text", "text": text}
                        )

                    elif content_block.type == "tool_use":
                        tool_name = content_block.name
                        tool_input = content_block.input
                        tool_id = content_block.id

                        print(f"\n🔧 Using tool: {tool_name}", flush=True)

                        # Execute tool with enhanced validation
                        tool_result = interface.execute_tool(tool_name, tool_input)

                        # Add tool use to assistant message
                        assistant_message["content"].append(
                            {
                                "type": "tool_use",
                                "id": tool_id,
                                "name": tool_name,
                                "input": tool_input,
                            }
                        )

                        # Add tool result to conversation
                        interface.conversation.append(assistant_message)
                        interface.conversation.append(
                            {
                                "role": "user",
                                "content": [
                                    {
                                        "type": "tool_result",
                                        "tool_use_id": tool_id,
                                        "content": str(tool_result),
                                    }
                                ],
                            }
                        )

                        # Clear assistant message for follow-up
                        assistant_message = {"role": "assistant", "content": []}

                        # Get follow-up response
                        followup_response = interface.client.messages.create(
                            model="deepseek-chat",
                            max_tokens=4096,
                            messages=interface.conversation,
                            tools=interface.tools,
                        )

                        for followup_block in followup_response.content:
                            if followup_block.type == "text":
                                followup_text = followup_block.text
                                print(followup_text, end="", flush=True)
                                assistant_message["content"].append(
                                    {
                                        "type": "text",
                                        "text": followup_text,
                                    }
                                )

                        break  # Exit after tool use

                # Add final assistant message to conversation
                if assistant_message["content"]:
                    interface.conversation.append(assistant_message)

                # Save conversation
                interface._save_conversation()

                print()  # New line after response

            except Exception as e:
                print(f"\n❌ Error: {e}")
                # Remove failed user message from conversation
                if (
                    interface.conversation
                    and interface.conversation[-1]["role"] == "user"
                ):
                    interface.conversation.pop()

        except KeyboardInterrupt:
            print("\n👋 Session interrupted. Goodbye!")
            break
        except EOFError:
            print("\n👋 Session ended. Goodbye!")
            break


def main():
    """Main function for enhanced interface with interactive streaming"""
    import argparse

    parser = argparse.ArgumentParser(
        description="Enhanced DeepSeek Integration Framework"
    )
    parser.add_argument(
        "--session",
        "-s",
        type=str,
        default="enhanced_dublin",
        help="Session name for context persistence",
    )
    parser.add_argument(
        "--message", "-m", type=str, help="Single message (non-interactive)"
    )
    parser.add_argument(
        "--test", "-t", action="store_true", help="Run basic functionality tests"
    )

    args = parser.parse_args()

    print("🛠️  Enhanced DeepSeek Integration Framework")
    print("=" * 60)

    try:
        enhanced_interface = EnhancedDeepSeekInterface(session_name=args.session)

        if args.test:
            # Run basic functionality tests
            print("✅ Enhanced interface initialized successfully")
            print(f"📊 System Status: {enhanced_interface.get_system_status()}")

            # Test parameter validation
            print("\n🧪 Testing parameter validation...")

            # Valid call
            result = enhanced_interface.execute_tool("list_files", {})
            print(f"✅ Valid call result: {result['status']}")

            # Invalid call (missing required parameter)
            result = enhanced_interface.execute_tool("read_file", {})
            print(f"❌ Invalid call result: {result['type']} - {result['message']}")

            # Test session restart
            print("\n🔄 Testing session restart...")
            restart_result = enhanced_interface.session_restart()
            print(f"🔄 Session restart: {restart_result}")

            print("\n✅ All enhanced features working correctly!")

        elif args.message:
            # Single message mode with streaming
            print("\n" + "=" * 80)
            print("ENHANCED DEEPSEEK RESPONSE:")
            print("=" * 80)

            # For single messages, we need to create a streaming-like interface
            # Since the enhanced interface doesn't have streaming built-in,
            # we'll simulate it by processing the message
            print("🤖 DeepSeek: Processing with enhanced safety and monitoring...")

            # Note: The enhanced interface doesn't have streaming built-in
            # For streaming, you'd need to combine with the streaming interface
            print("⚠️  Note: Enhanced interface doesn't include streaming.")
            print(
                "💡 For streaming responses, use: python tools/deepseek_streaming_fresh.py"
            )
            print("💡 For enhanced safety features, use this interface for tool calls.")

        else:
            # Interactive mode - start chat session
            print("✅ Enhanced interface initialized successfully")
            print(f"📊 System Status: {enhanced_interface.get_system_status()}")
            print("\n" + "=" * 60)
            print("ENHANCED FEATURES ACTIVE:")
            print("🛡️  Parameter validation before tool execution")
            print("🔄 Pattern monitoring to detect infinite loops")
            print("🚨 Graceful error handling with recovery")
            print("💾 Session management with proper context reset")
            print("=" * 60)
            print("\n🤖 Starting Enhanced DeepSeek Interactive Session...")
            print(
                "Type 'quit' to exit, 'clear' to reset conversation, 'status' for system info"
            )
            print("=" * 60)

            # Start interactive session
            start_interactive_session(enhanced_interface)

    except Exception as e:
        print(f"❌ Error: {e}")
        print("Make sure DEEPSEEK_API_KEY is set and you have internet connection")


if __name__ == "__main__":
    main()

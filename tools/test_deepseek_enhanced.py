#!/usr/bin/env python3
"""
Comprehensive Test Suite for Enhanced DeepSeek Integration Framework

Tests all components thoroughly to ensure robustness and reliability:
- Parameter validation edge cases
- Execution pattern monitoring
- Error handling and recovery
- Session management
- Security validations
- Tool execution scenarios
- Integration testing
"""

import os
import sys
import json
import time
import tempfile
import shutil
from pathlib import Path
from typing import Dict, List

# Add project root to path
project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

from tools.deepseek_enhanced import (
    EnhancedDeepSeekInterface,
    ParameterValidator,
    SessionManager,
    GracefulErrorHandler,
    ExecutionPatternMonitor,
    ErrorSeverity,
    ExecutionPattern,
)


class EnhancedFrameworkTester:
    """Comprehensive test suite for enhanced framework"""

    def __init__(self):
        self.test_results = []
        self.test_dir = Path(tempfile.mkdtemp())
        self.errors = []
        self.warnings = []

    def log_test(self, test_name: str, status: str, message: str = ""):
        """Log test result"""
        result = {
            "test": test_name,
            "status": status,
            "message": message,
            "timestamp": time.time(),
        }
        self.test_results.append(result)

        if status == "PASS":
            print(f"✅ {test_name}: {message}")
        elif status == "FAIL":
            print(f"❌ {test_name}: {message}")
            self.errors.append(f"{test_name}: {message}")
        elif status == "WARN":
            print(f"⚠️  {test_name}: {message}")
            self.warnings.append(f"{test_name}: {message}")

    def test_parameter_validator(self) -> bool:
        """Test parameter validation comprehensively"""
        print("\n🧪 Testing Parameter Validator...")

        validator = ParameterValidator()
        all_passed = True

        # Test 1: Valid parameters
        try:
            is_valid, msg = validator.validate("list_files", {})
            if is_valid:
                self.log_test(
                    "ParameterValidator_ValidCall", "PASS", "Valid parameters accepted"
                )
            else:
                self.log_test(
                    "ParameterValidator_ValidCall",
                    "FAIL",
                    f"Valid parameters rejected: {msg}",
                )
                all_passed = False
        except Exception as e:
            self.log_test("ParameterValidator_ValidCall", "FAIL", f"Exception: {e}")
            all_passed = False

        # Test 2: Missing required parameters
        try:
            is_valid, msg = validator.validate("read_file", {})
            if not is_valid and "Missing required parameter" in msg:
                self.log_test(
                    "ParameterValidator_MissingParam",
                    "PASS",
                    "Missing parameters detected",
                )
            else:
                self.log_test(
                    "ParameterValidator_MissingParam",
                    "FAIL",
                    f"Missing parameters not detected: {msg}",
                )
                all_passed = False
        except Exception as e:
            self.log_test("ParameterValidator_MissingParam", "FAIL", f"Exception: {e}")
            all_passed = False

        # Test 3: Wrong parameter types
        try:
            is_valid, msg = validator.validate("read_file", {"file_path": 123})
            if not is_valid and "must be a string" in msg:
                self.log_test(
                    "ParameterValidator_WrongType", "PASS", "Wrong types detected"
                )
            else:
                self.log_test(
                    "ParameterValidator_WrongType",
                    "FAIL",
                    f"Wrong types not detected: {msg}",
                )
                all_passed = False
        except Exception as e:
            self.log_test("ParameterValidator_WrongType", "FAIL", f"Exception: {e}")
            all_passed = False

        # Test 4: Dangerous bash commands
        try:
            is_valid, msg = validator.validate("execute_bash", {"command": "rm -rf /"})
            if not is_valid and "dangerous" in msg.lower():
                self.log_test(
                    "ParameterValidator_DangerousCommand",
                    "PASS",
                    "Dangerous commands blocked",
                )
            else:
                self.log_test(
                    "ParameterValidator_DangerousCommand",
                    "FAIL",
                    f"Dangerous commands not blocked: {msg}",
                )
                all_passed = False
        except Exception as e:
            self.log_test(
                "ParameterValidator_DangerousCommand", "FAIL", f"Exception: {e}"
            )
            all_passed = False

        # Test 5: Directory traversal attempts
        try:
            is_valid, msg = validator.validate(
                "read_file", {"file_path": "../../../etc/passwd"}
            )
            if not is_valid and "traversal" in msg.lower():
                self.log_test(
                    "ParameterValidator_DirectoryTraversal",
                    "PASS",
                    "Directory traversal blocked",
                )
            else:
                self.log_test(
                    "ParameterValidator_DirectoryTraversal",
                    "FAIL",
                    f"Directory traversal not blocked: {msg}",
                )
                all_passed = False
        except Exception as e:
            self.log_test(
                "ParameterValidator_DirectoryTraversal", "FAIL", f"Exception: {e}"
            )
            all_passed = False

        # Test 6: Absolute paths
        try:
            is_valid, msg = validator.validate(
                "read_file", {"file_path": "/etc/passwd"}
            )
            if not is_valid and "absolute paths" in msg.lower():
                self.log_test(
                    "ParameterValidator_AbsolutePath", "PASS", "Absolute paths blocked"
                )
            else:
                self.log_test(
                    "ParameterValidator_AbsolutePath",
                    "FAIL",
                    f"Absolute paths not blocked: {msg}",
                )
                all_passed = False
        except Exception as e:
            self.log_test("ParameterValidator_AbsolutePath", "FAIL", f"Exception: {e}")
            all_passed = False

        return all_passed

    def test_execution_pattern_monitor(self) -> bool:
        """Test execution pattern monitoring"""
        print("\n📊 Testing Execution Pattern Monitor...")

        monitor = ExecutionPatternMonitor()
        all_passed = True

        # Test 1: Normal pattern detection
        try:
            monitor.record_call("list_files", {}, True, execution_time=0.1)
            monitor.record_call(
                "read_file", {"file_path": "test.cpp"}, True, execution_time=0.2
            )

            is_invalid, msg = monitor.detect_invalid_pattern()
            if not is_invalid and "normal" in msg.lower():
                self.log_test(
                    "PatternMonitor_NormalPattern",
                    "PASS",
                    "Normal pattern correctly identified",
                )
            else:
                self.log_test(
                    "PatternMonitor_NormalPattern",
                    "FAIL",
                    f"Normal pattern misidentified: {msg}",
                )
                all_passed = False
        except Exception as e:
            self.log_test("PatternMonitor_NormalPattern", "FAIL", f"Exception: {e}")
            all_passed = False

        # Test 2: Repetitive calls detection
        try:
            monitor.reset_patterns()
            for _ in range(5):
                monitor.record_call(
                    "read_file", {"file_path": "same_file.cpp"}, False, "File not found"
                )

            is_invalid, msg = monitor.detect_invalid_pattern()
            if is_invalid and "repetitive" in msg.lower():
                self.log_test(
                    "PatternMonitor_RepetitiveCalls",
                    "PASS",
                    "Repetitive calls detected",
                )
            else:
                self.log_test(
                    "PatternMonitor_RepetitiveCalls",
                    "FAIL",
                    f"Repetitive calls not detected: {msg}",
                )
                all_passed = False
        except Exception as e:
            self.log_test("PatternMonitor_RepetitiveCalls", "FAIL", f"Exception: {e}")
            all_passed = False

        # Test 3: Infinite loop detection
        try:
            monitor.reset_patterns()
            for _ in range(6):
                monitor.record_call("list_files", {}, False, "Permission denied")

            is_invalid, msg = monitor.detect_invalid_pattern()
            if is_invalid and "loop" in msg.lower():
                self.log_test(
                    "PatternMonitor_InfiniteLoop", "PASS", "Infinite loop detected"
                )
            else:
                self.log_test(
                    "PatternMonitor_InfiniteLoop",
                    "FAIL",
                    f"Infinite loop not detected: {msg}",
                )
                all_passed = False
        except Exception as e:
            self.log_test("PatternMonitor_InfiniteLoop", "FAIL", f"Exception: {e}")
            all_passed = False

        # Test 4: Pattern reset
        try:
            monitor.reset_patterns()
            status = monitor.get_pattern_status()
            if status["error_count"] == 0 and status["success_count"] == 0:
                self.log_test(
                    "PatternMonitor_Reset", "PASS", "Pattern reset works correctly"
                )
            else:
                self.log_test(
                    "PatternMonitor_Reset", "FAIL", f"Pattern reset failed: {status}"
                )
                all_passed = False
        except Exception as e:
            self.log_test("PatternMonitor_Reset", "FAIL", f"Exception: {e}")
            all_passed = False

        return all_passed

    def test_error_handler(self) -> bool:
        """Test error handling and recovery"""
        print("\n🚨 Testing Error Handler...")

        handler = GracefulErrorHandler()
        all_passed = True

        # Test 1: Parameter error handling
        try:
            error_info = handler.parameter_error("read_file", "Missing file_path")
            if (
                error_info["type"] == "parameter_error"
                and error_info["severity"] == ErrorSeverity.HIGH
                and error_info["recoverable"] == True
            ):
                self.log_test(
                    "ErrorHandler_ParameterError",
                    "PASS",
                    "Parameter errors handled correctly",
                )
            else:
                self.log_test(
                    "ErrorHandler_ParameterError",
                    "FAIL",
                    f"Parameter error handling failed: {error_info}",
                )
                all_passed = False
        except Exception as e:
            self.log_test("ErrorHandler_ParameterError", "FAIL", f"Exception: {e}")
            all_passed = False

        # Test 2: Pattern reset handling
        try:
            error_info = handler.pattern_reset()
            if (
                error_info["type"] == "pattern_reset"
                and error_info["severity"] == ErrorSeverity.CRITICAL
                and error_info["action"] == "reset_patterns"
            ):
                self.log_test(
                    "ErrorHandler_PatternReset",
                    "PASS",
                    "Pattern reset handled correctly",
                )
            else:
                self.log_test(
                    "ErrorHandler_PatternReset",
                    "FAIL",
                    f"Pattern reset handling failed: {error_info}",
                )
                all_passed = False
        except Exception as e:
            self.log_test("ErrorHandler_PatternReset", "FAIL", f"Exception: {e}")
            all_passed = False

        # Test 3: Exception handling
        try:
            error_info = handler.graceful_error(ValueError("Test error"))
            if (
                error_info["type"] == "execution_error"
                and error_info["exception_type"] == "ValueError"
                and error_info["recoverable"] == True
            ):
                self.log_test(
                    "ErrorHandler_Exception", "PASS", "Exceptions handled correctly"
                )
            else:
                self.log_test(
                    "ErrorHandler_Exception",
                    "FAIL",
                    f"Exception handling failed: {error_info}",
                )
                all_passed = False
        except Exception as e:
            self.log_test("ErrorHandler_Exception", "FAIL", f"Exception: {e}")
            all_passed = False

        # Test 4: Error severity classification
        try:
            critical_error = handler.graceful_error(ConnectionError("Network failed"))
            high_error = handler.graceful_error(ValueError("Invalid value"))
            medium_error = handler.graceful_error(FileNotFoundError("File missing"))

            if (
                critical_error["severity"] == ErrorSeverity.CRITICAL
                and high_error["severity"] == ErrorSeverity.HIGH
                and medium_error["severity"] == ErrorSeverity.MEDIUM
            ):
                self.log_test(
                    "ErrorHandler_SeverityClassification",
                    "PASS",
                    "Error severity classified correctly",
                )
            else:
                self.log_test(
                    "ErrorHandler_SeverityClassification",
                    "FAIL",
                    "Error severity classification failed",
                )
                all_passed = False
        except Exception as e:
            self.log_test(
                "ErrorHandler_SeverityClassification", "FAIL", f"Exception: {e}"
            )
            all_passed = False

        return all_passed

    def test_session_manager(self) -> bool:
        """Test session management"""
        print("\n💾 Testing Session Manager...")

        # Create test session directory
        test_context_dir = self.test_dir / "test_sessions"
        test_context_dir.mkdir(exist_ok=True)

        manager = SessionManager(test_context_dir)
        all_passed = True

        # Test 1: Session creation
        try:
            session_data = manager.create_session("test_session", "Test context")
            if (
                session_data["enhanced_mode"] == True
                and session_data["initial_context"] == "Test context"
            ):
                self.log_test(
                    "SessionManager_CreateSession", "PASS", "Session creation works"
                )
            else:
                self.log_test(
                    "SessionManager_CreateSession",
                    "FAIL",
                    f"Session creation failed: {session_data}",
                )
                all_passed = False
        except Exception as e:
            self.log_test("SessionManager_CreateSession", "FAIL", f"Exception: {e}")
            all_passed = False

        # Test 2: Context clearing
        try:
            # Create dummy files
            conv_file = test_context_dir / "test_session_conversation.json"
            sess_file = test_context_dir / "test_session_session.pkl"
            conv_file.write_text("{}")
            sess_file.write_text("dummy")

            success = manager.clear_context("test_session")
            if success and not conv_file.exists() and not sess_file.exists():
                self.log_test(
                    "SessionManager_ClearContext", "PASS", "Context clearing works"
                )
            else:
                self.log_test(
                    "SessionManager_ClearContext", "FAIL", "Context clearing failed"
                )
                all_passed = False
        except Exception as e:
            self.log_test("SessionManager_ClearContext", "FAIL", f"Exception: {e}")
            all_passed = False

        # Test 3: Session metadata persistence
        try:
            manager.create_session("persistent_test")
            sessions_file = test_context_dir / "enhanced_sessions.json"

            if sessions_file.exists():
                with open(sessions_file, "r") as f:
                    sessions_data = json.load(f)
                if "persistent_test" in sessions_data:
                    self.log_test(
                        "SessionManager_Persistence",
                        "PASS",
                        "Session metadata persisted",
                    )
                else:
                    self.log_test(
                        "SessionManager_Persistence",
                        "FAIL",
                        "Session metadata not persisted",
                    )
                    all_passed = False
            else:
                self.log_test(
                    "SessionManager_Persistence", "FAIL", "Sessions file not created"
                )
                all_passed = False
        except Exception as e:
            self.log_test("SessionManager_Persistence", "FAIL", f"Exception: {e}")
            all_passed = False

        return all_passed

    def test_enhanced_interface_integration(self) -> bool:
        """Test full enhanced interface integration"""
        print("\n🔗 Testing Enhanced Interface Integration...")

        # Create temporary test environment
        test_session_dir = self.test_dir / "integration_test"
        test_session_dir.mkdir(exist_ok=True)

        # Mock the API key for testing
        original_api_key = os.environ.get("DEEPSEEK_API_KEY")
        os.environ["DEEPSEEK_API_KEY"] = "test_key_12345"

        all_passed = True

        try:
            # Test 1: Interface initialization
            try:
                interface = EnhancedDeepSeekInterface("integration_test")
                if interface.session_name == "integration_test":
                    self.log_test(
                        "EnhancedInterface_Init",
                        "PASS",
                        "Interface initialization works",
                    )
                else:
                    self.log_test(
                        "EnhancedInterface_Init",
                        "FAIL",
                        "Interface initialization failed",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test("EnhancedInterface_Init", "FAIL", f"Exception: {e}")
                all_passed = False

            # Test 2: Tool execution with valid parameters
            try:
                result = interface.execute_tool("list_files", {})
                if result["status"] == "success":
                    self.log_test(
                        "EnhancedInterface_ValidToolCall",
                        "PASS",
                        "Valid tool calls work",
                    )
                else:
                    self.log_test(
                        "EnhancedInterface_ValidToolCall",
                        "FAIL",
                        f"Valid tool call failed: {result}",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test(
                    "EnhancedInterface_ValidToolCall", "FAIL", f"Exception: {e}"
                )
                all_passed = False

            # Test 3: Tool execution with invalid parameters
            try:
                result = interface.execute_tool("read_file", {})
                if result["type"] == "parameter_error":
                    self.log_test(
                        "EnhancedInterface_InvalidToolCall",
                        "PASS",
                        "Invalid tool calls handled correctly",
                    )
                else:
                    self.log_test(
                        "EnhancedInterface_InvalidToolCall",
                        "FAIL",
                        f"Invalid tool call not handled: {result}",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test(
                    "EnhancedInterface_InvalidToolCall", "FAIL", f"Exception: {e}"
                )
                all_passed = False

            # Test 4: Pattern monitoring integration
            try:
                # Make several calls to trigger pattern detection
                for _ in range(6):
                    interface.execute_tool(
                        "read_file", {"file_path": "nonexistent.cpp"}
                    )

                metrics = interface.execution_monitor.get_pattern_status()
                if metrics["error_count"] >= 6:
                    self.log_test(
                        "EnhancedInterface_PatternMonitoring",
                        "PASS",
                        "Pattern monitoring integrated correctly",
                    )
                else:
                    self.log_test(
                        "EnhancedInterface_PatternMonitoring",
                        "FAIL",
                        "Pattern monitoring not working",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test(
                    "EnhancedInterface_PatternMonitoring", "FAIL", f"Exception: {e}"
                )
                all_passed = False

            # Test 5: Session restart functionality
            try:
                restart_result = interface.session_restart()
                if "complete" in restart_result.lower():
                    self.log_test(
                        "EnhancedInterface_SessionRestart",
                        "PASS",
                        "Session restart works",
                    )
                else:
                    self.log_test(
                        "EnhancedInterface_SessionRestart",
                        "FAIL",
                        f"Session restart failed: {restart_result}",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test(
                    "EnhancedInterface_SessionRestart", "FAIL", f"Exception: {e}"
                )
                all_passed = False

            # Test 6: System status reporting
            try:
                status = interface.get_system_status()
                required_keys = [
                    "session_name",
                    "enhanced_mode",
                    "components_status",
                    "execution_metrics",
                ]
                if all(key in status for key in required_keys):
                    self.log_test(
                        "EnhancedInterface_SystemStatus",
                        "PASS",
                        "System status reporting works",
                    )
                else:
                    self.log_test(
                        "EnhancedInterface_SystemStatus",
                        "FAIL",
                        "System status reporting incomplete",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test(
                    "EnhancedInterface_SystemStatus", "FAIL", f"Exception: {e}"
                )
                all_passed = False

        finally:
            # Restore original API key
            if original_api_key is None:
                os.environ.pop("DEEPSEEK_API_KEY", None)
            else:
                os.environ["DEEPSEEK_API_KEY"] = original_api_key

        return all_passed

    def test_tool_execution_scenarios(self) -> bool:
        """Test various tool execution scenarios"""
        print("\n🔧 Testing Tool Execution Scenarios...")

        # Mock API key
        original_api_key = os.environ.get("DEEPSEEK_API_KEY")
        os.environ["DEEPSEEK_API_KEY"] = "test_key_12345"

        all_passed = True

        try:
            interface = EnhancedDeepSeekInterface("scenario_test")

            # Test 1: File operations with valid paths
            try:
                # Create test file
                test_file = self.test_dir / "test_scenario.txt"
                test_file.write_text("Test content for scenario testing")

                # Test read file
                result = interface.execute_tool(
                    "read_file", {"file_path": str(test_file)}
                )
                if result.get("status") == "success" and "Test content" in result.get(
                    "result", ""
                ):
                    self.log_test(
                        "ToolExecution_ReadFile", "PASS", "File reading works"
                    )
                else:
                    self.log_test(
                        "ToolExecution_ReadFile",
                        "FAIL",
                        f"File reading failed: {result}",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test("ToolExecution_ReadFile", "FAIL", f"Exception: {e}")
                all_passed = False

            # Test 2: File operations with invalid paths
            try:
                result = interface.execute_tool(
                    "read_file", {"file_path": "nonexistent_file_12345.txt"}
                )
                if result.get("status") == "error":
                    self.log_test(
                        "ToolExecution_InvalidFile",
                        "PASS",
                        "Invalid file handling works",
                    )
                else:
                    self.log_test(
                        "ToolExecution_InvalidFile",
                        "FAIL",
                        f"Invalid file handling failed: {result}",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test("ToolExecution_InvalidFile", "FAIL", f"Exception: {e}")
                all_passed = False

            # Test 3: Write file operations
            try:
                write_path = str(self.test_dir / "written_file.txt")
                result = interface.execute_tool(
                    "write_file",
                    {
                        "file_path": write_path,
                        "content": "Written by enhanced framework test",
                    },
                )
                if result.get(
                    "status"
                ) == "success" and "Successfully wrote" in result.get("result", ""):
                    # Verify file was actually written
                    if (
                        Path(write_path).read_text()
                        == "Written by enhanced framework test"
                    ):
                        self.log_test(
                            "ToolExecution_WriteFile", "PASS", "File writing works"
                        )
                    else:
                        self.log_test(
                            "ToolExecution_WriteFile",
                            "FAIL",
                            "File writing verification failed",
                        )
                        all_passed = False
                else:
                    self.log_test(
                        "ToolExecution_WriteFile",
                        "FAIL",
                        f"File writing failed: {result}",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test("ToolExecution_WriteFile", "FAIL", f"Exception: {e}")
                all_passed = False

            # Test 4: List files with different directories
            try:
                result = interface.execute_tool("list_files", {"directory": "."})
                if result["status"] == "success":
                    self.log_test(
                        "ToolExecution_ListFiles", "PASS", "Directory listing works"
                    )
                else:
                    self.log_test(
                        "ToolExecution_ListFiles", "FAIL", "Directory listing failed"
                    )
                    all_passed = False
            except Exception as e:
                self.log_test("ToolExecution_ListFiles", "FAIL", f"Exception: {e}")
                all_passed = False

            # Test 5: Git status (may fail if not in git repo, but should handle gracefully)
            try:
                result = interface.execute_tool("git_status", {})
                # Should either succeed or fail gracefully
                if result["status"] in ["success", "error"]:
                    self.log_test(
                        "ToolExecution_GitStatus",
                        "PASS",
                        "Git status handled gracefully",
                    )
                else:
                    self.log_test(
                        "ToolExecution_GitStatus", "FAIL", "Git status handling failed"
                    )
                    all_passed = False
            except Exception as e:
                self.log_test("ToolExecution_GitStatus", "FAIL", f"Exception: {e}")
                all_passed = False

        finally:
            # Restore original API key
            if original_api_key is None:
                os.environ.pop("DEEPSEEK_API_KEY", None)
            else:
                os.environ["DEEPSEEK_API_KEY"] = original_api_key

        return all_passed

    def test_edge_cases_and_stress(self) -> bool:
        """Test edge cases and stress scenarios"""
        print("\n⚡ Testing Edge Cases and Stress...")

        # Mock API key
        original_api_key = os.environ.get("DEEPSEEK_API_KEY")
        os.environ["DEEPSEEK_API_KEY"] = "test_key_12345"

        all_passed = True

        try:
            interface = EnhancedDeepSeekInterface("stress_test")

            # Test 1: Rapid successive calls
            try:
                for i in range(10):
                    interface.execute_tool("list_files", {})

                metrics = interface.execution_monitor.get_pattern_status()
                if metrics["success_count"] >= 8:  # Allow some tolerance
                    self.log_test(
                        "StressTest_RapidCalls", "PASS", "Rapid calls handled correctly"
                    )
                else:
                    self.log_test(
                        "StressTest_RapidCalls",
                        "FAIL",
                        "Rapid calls not handled properly",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test("StressTest_RapidCalls", "FAIL", f"Exception: {e}")
                all_passed = False

            # Test 2: Mixed valid and invalid calls
            try:
                results = []
                for i in range(5):
                    # Alternate between valid and invalid calls
                    if i % 2 == 0:
                        result = interface.execute_tool("list_files", {})
                    else:
                        result = interface.execute_tool("read_file", {})

                    results.append(result.get("status", "unknown"))

                success_count = results.count("success")
                error_count = results.count("error") + results.count("parameter_error")

                if success_count >= 2 and error_count >= 2:
                    self.log_test(
                        "StressTest_MixedCalls",
                        "PASS",
                        "Mixed valid/invalid calls handled",
                    )
                else:
                    self.log_test(
                        "StressTest_MixedCalls",
                        "FAIL",
                        f"Mixed calls not handled correctly: {results}",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test("StressTest_MixedCalls", "FAIL", f"Exception: {e}")
                all_passed = False

            # Test 3: Empty and malformed parameters
            try:
                # Test empty parameters for tools that require them
                result1 = interface.execute_tool("read_file", {"file_path": ""})
                result2 = interface.execute_tool(
                    "write_file", {"file_path": "test", "content": ""}
                )

                if (
                    result1["type"] == "parameter_error"
                    and result2["status"] == "success"
                ):
                    self.log_test(
                        "StressTest_EmptyParams",
                        "PASS",
                        "Empty parameters handled correctly",
                    )
                else:
                    self.log_test(
                        "StressTest_EmptyParams",
                        "FAIL",
                        "Empty parameters not handled correctly",
                    )
                    all_passed = False
            except Exception as e:
                self.log_test("StressTest_EmptyParams", "FAIL", f"Exception: {e}")
                all_passed = False

            # Test 4: Very long parameter values
            try:
                long_content = "x" * 10000  # 10KB of content
                result = interface.execute_tool(
                    "write_file",
                    {
                        "file_path": str(self.test_dir / "long_content.txt"),
                        "content": long_content,
                    },
                )

                if result["status"] == "success":
                    # Verify content was written correctly
                    written_content = Path(
                        self.test_dir / "long_content.txt"
                    ).read_text()
                    if written_content == long_content:
                        self.log_test(
                            "StressTest_LongContent",
                            "PASS",
                            "Long content handled correctly",
                        )
                    else:
                        self.log_test(
                            "StressTest_LongContent",
                            "FAIL",
                            "Long content not written correctly",
                        )
                        all_passed = False
                else:
                    self.log_test(
                        "StressTest_LongContent", "FAIL", "Long content handling failed"
                    )
                    all_passed = False
            except Exception as e:
                self.log_test("StressTest_LongContent", "FAIL", f"Exception: {e}")
                all_passed = False

        finally:
            # Restore original API key
            if original_api_key is None:
                os.environ.pop("DEEPSEEK_API_KEY", None)
            else:
                os.environ["DEEPSEEK_API_KEY"] = original_api_key

        return all_passed

    def run_all_tests(self) -> bool:
        """Run all test suites"""
        print("🧪 COMPREHENSIVE ENHANCED FRAMEWORK TEST SUITE")
        print("=" * 60)

        test_methods = [
            self.test_parameter_validator,
            self.test_execution_pattern_monitor,
            self.test_error_handler,
            self.test_session_manager,
            self.test_enhanced_interface_integration,
            self.test_tool_execution_scenarios,
            self.test_edge_cases_and_stress,
        ]

        passed_tests = 0
        total_tests = len(test_methods)

        for test_method in test_methods:
            try:
                if test_method():
                    passed_tests += 1
            except Exception as e:
                self.log_test(
                    f"{test_method.__name__}_Exception",
                    "FAIL",
                    f"Test suite crashed: {e}",
                )

        # Print summary
        print("\n" + "=" * 60)
        print("📊 TEST SUMMARY")
        print("=" * 60)
        print(f"Tests Passed: {passed_tests}/{total_tests}")
        print(f"Tests Failed: {total_tests - passed_tests}")

        if self.errors:
            print(f"\n❌ ERRORS ({len(self.errors)}):")
            for error in self.errors:
                print(f"  - {error}")

        if self.warnings:
            print(f"\n⚠️  WARNINGS ({len(self.warnings)}):")
            for warning in self.warnings:
                print(f"  - {warning}")

        success_rate = (passed_tests / total_tests) * 100
        print(f"\n🎯 SUCCESS RATE: {success_rate:.1f}%")

        if passed_tests == total_tests:
            print("🎉 ALL TESTS PASSED! Framework is production-ready!")
            return True
        else:
            print("⚠️  SOME TESTS FAILED! Framework needs fixes before production use.")
            return False

    def cleanup(self):
        """Clean up test files"""
        try:
            shutil.rmtree(self.test_dir)
            print(f"\n🧹 Cleaned up test directory: {self.test_dir}")
        except Exception as e:
            print(f"\n⚠️  Could not clean up test directory: {e}")


def main():
    """Main test function"""
    tester = EnhancedFrameworkTester()

    try:
        success = tester.run_all_tests()

        if success:
            print("\n🎉 ENHANCED FRAMEWORK IS FULLY VALIDATED AND PRODUCTION-READY!")
        else:
            print("\n⚠️  FRAMEWORK NEEDS IMPROVEMENTS BEFORE PRODUCTION USE")

        return success

    finally:
        tester.cleanup()


if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)

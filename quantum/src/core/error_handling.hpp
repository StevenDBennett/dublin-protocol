#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

#include <stdexcept>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <iostream>
#include <chrono>
#include <iomanip>

namespace dublin_error {

// === Error Categories ===

enum class ErrorCategory {
    MEMORY_SAFETY,
    COMPUTATION,
    HARDWARE,
    PERFORMANCE,
    VALIDATION,
    CONFIGURATION,
    UNKNOWN
};

// === Error Severity Levels ===

enum class ErrorSeverity {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// === Custom Exception Classes ===

class DublinException : public std::exception {
private:
    std::string message;
    ErrorCategory category;
    ErrorSeverity severity;
    std::chrono::system_clock::time_point timestamp;

public:
    DublinException(const std::string& msg,
                   ErrorCategory cat = ErrorCategory::UNKNOWN,
                   ErrorSeverity sev = ErrorSeverity::ERROR)
        : message(msg), category(cat), severity(sev),
          timestamp(std::chrono::system_clock::now()) {}

    const char* what() const noexcept override {
        return message.c_str();
    }

    ErrorCategory get_category() const { return category; }
    ErrorSeverity get_severity() const { return severity; }
    auto get_timestamp() const { return timestamp; }

    std::string to_string() const {
        std::stringstream ss;
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        ss << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] "
           << "[" << severity_to_string(severity) << "] "
           << "[" << category_to_string(category) << "] "
           << message;
        return ss.str();
    }

private:
    static std::string severity_to_string(ErrorSeverity sev) {
        switch (sev) {
            case ErrorSeverity::DEBUG: return "DEBUG";
            case ErrorSeverity::INFO: return "INFO";
            case ErrorSeverity::WARNING: return "WARNING";
            case ErrorSeverity::ERROR: return "ERROR";
            case ErrorSeverity::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }

    static std::string category_to_string(ErrorCategory cat) {
        switch (cat) {
            case ErrorCategory::MEMORY_SAFETY: return "MEMORY_SAFETY";
            case ErrorCategory::COMPUTATION: return "COMPUTATION";
            case ErrorCategory::HARDWARE: return "HARDWARE";
            case ErrorCategory::PERFORMANCE: return "PERFORMANCE";
            case ErrorCategory::VALIDATION: return "VALIDATION";
            case ErrorCategory::CONFIGURATION: return "CONFIGURATION";
            default: return "UNKNOWN";
        }
    }
};

// === Specific Exception Types ===

class MemorySafetyException : public DublinException {
public:
    MemorySafetyException(const std::string& msg, ErrorSeverity sev = ErrorSeverity::CRITICAL)
        : DublinException(msg, ErrorCategory::MEMORY_SAFETY, sev) {}
};

class ComputationException : public DublinException {
public:
    ComputationException(const std::string& msg, ErrorSeverity sev = ErrorSeverity::ERROR)
        : DublinException(msg, ErrorCategory::COMPUTATION, sev) {}
};

class HardwareException : public DublinException {
public:
    HardwareException(const std::string& msg, ErrorSeverity sev = ErrorSeverity::WARNING)
        : DublinException(msg, ErrorCategory::HARDWARE, sev) {}
};

class PerformanceException : public DublinException {
public:
    PerformanceException(const std::string& msg, ErrorSeverity sev = ErrorSeverity::WARNING)
        : DublinException(msg, ErrorCategory::PERFORMANCE, sev) {}
};

class ValidationException : public DublinException {
public:
    ValidationException(const std::string& msg, ErrorSeverity sev = ErrorSeverity::ERROR)
        : DublinException(msg, ErrorCategory::VALIDATION, sev) {}
};

// === Error Handler ===

class ErrorHandler {
private:
    static std::vector<std::string> error_log;
    static size_t max_log_size;
    static bool verbose_mode;

public:
    static void set_max_log_size(size_t size) {
        max_log_size = size;
    }

    static void set_verbose_mode(bool verbose) {
        verbose_mode = verbose;
    }

    static void handle_exception(const DublinException& e) {
        std::string error_str = e.to_string();

        // Add to log
        error_log.push_back(error_str);

        // Maintain log size
        if (error_log.size() > max_log_size) {
            error_log.erase(error_log.begin());
        }

        // Output based on severity
        switch (e.get_severity()) {
            case ErrorSeverity::CRITICAL:
            case ErrorSeverity::ERROR:
                std::cerr << "❌ " << error_str << std::endl;
                break;
            case ErrorSeverity::WARNING:
                std::cout << "⚠️  " << error_str << std::endl;
                break;
            case ErrorSeverity::INFO:
                if (verbose_mode) {
                    std::cout << "ℹ️  " << error_str << std::endl;
                }
                break;
            case ErrorSeverity::DEBUG:
                if (verbose_mode) {
                    std::cout << "🔍 " << error_str << std::endl;
                }
                break;
        }
    }

    static void log_info(const std::string& message) {
        DublinException e(message, ErrorCategory::UNKNOWN, ErrorSeverity::INFO);
        handle_exception(e);
    }

    static void log_warning(const std::string& message) {
        DublinException e(message, ErrorCategory::UNKNOWN, ErrorSeverity::WARNING);
        handle_exception(e);
    }

    static void log_error(const std::string& message) {
        DublinException e(message, ErrorCategory::UNKNOWN, ErrorSeverity::ERROR);
        handle_exception(e);
    }

    static std::vector<std::string> get_error_log() {
        return error_log;
    }

    static void clear_log() {
        error_log.clear();
    }

    static void print_summary() {
        std::cout << "\n=== ERROR HANDLING SUMMARY ===\n";
        std::cout << "Total logged errors: " << error_log.size() << "\n";

        if (!error_log.empty()) {
            std::cout << "\nRecent errors:\n";
            size_t count = std::min(error_log.size(), size_t(5));
            for (size_t i = error_log.size() - count; i < error_log.size(); ++i) {
                std::cout << "  " << error_log[i] << "\n";
            }
        }
        std::cout << "==============================\n";
    }
};

// Static member initialization
std::vector<std::string> ErrorHandler::error_log;
size_t ErrorHandler::max_log_size = 1000;
bool ErrorHandler::verbose_mode = false;

// === Safety Validators ===

class SafetyValidator {
public:
    // Memory safety validation
    static void validate_memory_access(const void* ptr, size_t size) {
        if (ptr == nullptr) {
            throw MemorySafetyException("Null pointer access attempted");
        }

        // Basic alignment check for AVX2
        if (reinterpret_cast<uintptr_t>(ptr) % 32 != 0) {
            ErrorHandler::log_warning("Unaligned memory access for AVX2 operations");
        }
    }

    // Vector size validation
    template<typename T>
    static void validate_vector_size(const std::vector<T>& vec, size_t min_size = 1) {
        if (vec.size() < min_size) {
            throw ValidationException("Vector size " + std::to_string(vec.size()) +
                                     " is less than minimum required " + std::to_string(min_size));
        }
    }

    // AVX2 operation validation
    static void validate_avx2_operation(size_t data_size, size_t elements_per_vector = 4) {
        if (data_size == 0) {
            throw ComputationException("AVX2 operation attempted on empty data");
        }

        if (data_size < elements_per_vector) {
            ErrorHandler::log_info("AVX2 operation on small dataset - using scalar fallback");
        }
    }

    // Performance validation
    static void validate_performance_threshold(double actual_time, double expected_time,
                                              const std::string& operation_name) {
        if (actual_time > expected_time * 1.5) {
            throw PerformanceException(operation_name + " performance degraded: " +
                                      std::to_string(actual_time) + "ms vs expected " +
                                      std::to_string(expected_time) + "ms");
        }
    }

    // Hardware capability validation
    static void validate_hardware_capability(bool has_avx2, bool has_cuda) {
        if (!has_avx2) {
            throw HardwareException("AVX2 not available - performance will be degraded");
        }

        if (!has_cuda) {
            ErrorHandler::log_info("CUDA not available - using CPU-only mode");
        }
    }
};

// === Safe Execution Wrapper ===

template<typename Func, typename... Args>
auto safe_execute(Func&& func, Args&&... args) -> decltype(func(args...)) {
    try {
        return func(std::forward<Args>(args)...);
    } catch (const DublinException& e) {
        ErrorHandler::handle_exception(e);
        throw;
    } catch (const std::exception& e) {
        DublinException wrapped(e.what(), ErrorCategory::UNKNOWN, ErrorSeverity::ERROR);
        ErrorHandler::handle_exception(wrapped);
        throw wrapped;
    } catch (...) {
        DublinException unknown("Unknown exception occurred", ErrorCategory::UNKNOWN, ErrorSeverity::CRITICAL);
        ErrorHandler::handle_exception(unknown);
        throw unknown;
    }
}

// === Error Recovery Strategies ===

class ErrorRecovery {
public:
    // Fallback to scalar implementation
    template<typename VectorType>
    static bool try_scalar_fallback(const std::string& operation_name,
                                   VectorType& result) {
        ErrorHandler::log_warning("Falling back to scalar implementation for: " + operation_name);
        // Implementation would depend on specific operation
        return true;
    }

    // Retry with smaller batch size
    template<typename Func>
    static auto retry_with_smaller_batch(Func&& func, size_t original_size,
                                        size_t& successful_size) -> decltype(func(original_size)) {
        for (size_t batch_size = original_size; batch_size >= 1; batch_size /= 2) {
            try {
                auto result = func(batch_size);
                successful_size = batch_size;
                ErrorHandler::log_info("Success with reduced batch size: " + std::to_string(batch_size));
                return result;
            } catch (const DublinException&) {
                // Continue trying with smaller batch
            }
        }
        throw ComputationException("Failed to execute even with minimum batch size");
    }

    // Graceful degradation
    static void degrade_gracefully(const std::string& feature_name) {
        ErrorHandler::log_warning("Gracefully degrading feature: " + feature_name);
        // Could disable specific optimizations or features
    }
};

} // namespace dublin_error

#endif // ERROR_HANDLING_HPP
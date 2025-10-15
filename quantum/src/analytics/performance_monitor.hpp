#pragma once
#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <numeric>

// 📊 PERFORMANCE MONITOR: Revolutionary Analytics & Optimization
// Tracks computational intelligence evolution and identifies optimization opportunities

namespace DublinProtocol {

class PerformanceMonitor {
private:
    struct AgentMetrics {
        std::string agent_name;
        uint64_t total_decisions{0};
        uint64_t successful_operations{0};
        double average_throughput{0.0};
        std::vector<double> throughput_history;
        std::chrono::steady_clock::time_point last_measurement;
    };

    std::unordered_map<std::string, AgentMetrics> agent_metrics;
    std::atomic<uint64_t> total_collective_operations{0};
    std::atomic<uint64_t> collective_intelligence_events{0};

    // Performance thresholds
    const double REVOLUTIONARY_THRESHOLD = 400000000.0; // 400M ops/sec
    const double INTELLIGENCE_THRESHOLD = 600000000.0;  // 600M ops/sec

public:
    PerformanceMonitor() = default;

    // Track individual agent performance
    void track_agent_performance(const std::string& agent_name, uint64_t operations,
                                double throughput, bool success = true) {
        auto& metrics = agent_metrics[agent_name];
        metrics.agent_name = agent_name;
        metrics.total_decisions += operations;
        if (success) metrics.successful_operations += operations;
        metrics.average_throughput = throughput;
        metrics.throughput_history.push_back(throughput);
        metrics.last_measurement = std::chrono::steady_clock::now();

        total_collective_operations += operations;

        // Detect intelligence events
        if (throughput > INTELLIGENCE_THRESHOLD) {
            collective_intelligence_events++;
            std::cout << "🧠 INTELLIGENCE EVENT: " << agent_name
                      << " achieved " << throughput << " ops/sec\n";
        }
    }

    // Analyze performance patterns
    void analyze_performance_patterns() {
        std::cout << "\n📊 REVOLUTIONARY PERFORMANCE ANALYSIS\n";
        std::cout << "===================================\n";

        for (const auto& [name, metrics] : agent_metrics) {
            std::cout << "\n🎯 " << name << " Agent Analysis:\n";
            std::cout << "  Total Decisions: " << metrics.total_decisions << "\n";
            std::cout << "  Success Rate: "
                      << (metrics.total_decisions > 0 ?
                         (double)metrics.successful_operations / metrics.total_decisions * 100 : 0)
                      << "%\n";
            std::cout << "  Current Throughput: " << metrics.average_throughput << " ops/sec\n";

            // Pattern analysis
            if (!metrics.throughput_history.empty()) {
                double max_throughput = *std::max_element(metrics.throughput_history.begin(),
                                                         metrics.throughput_history.end());
                double avg_throughput = std::accumulate(metrics.throughput_history.begin(),
                                                       metrics.throughput_history.end(), 0.0)
                                      / metrics.throughput_history.size();

                std::cout << "  Max Throughput: " << max_throughput << " ops/sec\n";
                std::cout << "  Average Throughput: " << avg_throughput << " ops/sec\n";

                // Revolutionary validation
                if (avg_throughput > REVOLUTIONARY_THRESHOLD) {
                    std::cout << "  ✅ REVOLUTIONARY PERFORMANCE CONFIRMED\n";
                }
                if (max_throughput > INTELLIGENCE_THRESHOLD) {
                    std::cout << "  🧠 COMPUTATIONAL INTELLIGENCE DETECTED\n";
                }

                // Trend analysis
                if (metrics.throughput_history.size() >= 2) {
                    double trend = metrics.throughput_history.back() - metrics.throughput_history.front();
                    if (trend > 0) {
                        std::cout << "  📈 POSITIVE PERFORMANCE TREND: +" << trend << " ops/sec\n";
                    } else if (trend < 0) {
                        std::cout << "  📉 NEGATIVE PERFORMANCE TREND: " << trend << " ops/sec\n";
                    }
                }
            }
        }

        // Collective intelligence assessment
        std::cout << "\n🤝 COLLECTIVE INTELLIGENCE ASSESSMENT\n";
        std::cout << "  Total Collective Operations: " << total_collective_operations << "\n";
        std::cout << "  Intelligence Events: " << collective_intelligence_events << "\n";

        if (collective_intelligence_events > 0) {
            std::cout << "  🚀 COLLECTIVE INTELLIGENCE ACTIVE!\n";
        }
    }

    // Identify optimization opportunities
    void identify_optimization_opportunities() {
        std::cout << "\n🔧 OPTIMIZATION OPPORTUNITIES\n";
        std::cout << "===========================\n";

        bool opportunities_found = false;

        for (const auto& [name, metrics] : agent_metrics) {
            if (!metrics.throughput_history.empty()) {
                double current = metrics.throughput_history.back();
                double max_historical = *std::max_element(metrics.throughput_history.begin(),
                                                         metrics.throughput_history.end());

                if (current < max_historical * 0.8) {
                    std::cout << "  • " << name << ": Performance degraded by "
                              << ((max_historical - current) / max_historical * 100) << "%\n";
                    std::cout << "    Recommendation: Investigate computational pathway optimization\n";
                    opportunities_found = true;
                }

                if (current < REVOLUTIONARY_THRESHOLD) {
                    std::cout << "  • " << name << ": Below revolutionary threshold by "
                              << (REVOLUTIONARY_THRESHOLD - current) << " ops/sec\n";
                    std::cout << "    Recommendation: Enhance hardware optimization\n";
                    opportunities_found = true;
                }
            }
        }

        if (!opportunities_found) {
            std::cout << "  ✅ ALL SYSTEMS OPERATING AT REVOLUTIONARY LEVELS!\n";
        }
    }

    // Predictive performance forecasting
    void forecast_performance_trends() {
        std::cout << "\n🔮 PERFORMANCE FORECASTING\n";
        std::cout << "========================\n";

        for (const auto& [name, metrics] : agent_metrics) {
            if (metrics.throughput_history.size() >= 3) {
                // Simple linear regression for trend prediction
                double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;
                int n = metrics.throughput_history.size();

                for (int i = 0; i < n; ++i) {
                    sum_x += i;
                    sum_y += metrics.throughput_history[i];
                    sum_xy += i * metrics.throughput_history[i];
                    sum_x2 += i * i;
                }

                double slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
                double predicted_growth = slope * 5; // Predict 5 cycles ahead

                std::cout << "  • " << name << ": Predicted change "
                          << (predicted_growth > 0 ? "+" : "") << predicted_growth << " ops/sec\n";

                if (predicted_growth > 10000000) { // 10M ops/sec growth
                    std::cout << "    🚀 SIGNIFICANT GROWTH FORECASTED!\n";
                }
            }
        }
    }

    // Generate comprehensive performance report
    void generate_comprehensive_report() {
        std::cout << "\n📈 REVOLUTIONARY PERFORMANCE REPORT\n";
        std::cout << "=================================\n";

        analyze_performance_patterns();
        identify_optimization_opportunities();
        forecast_performance_trends();

        std::cout << "\n🎯 OVERALL ASSESSMENT:\n";
        bool all_revolutionary = true;
        for (const auto& [name, metrics] : agent_metrics) {
            if (!metrics.throughput_history.empty() &&
                metrics.throughput_history.back() < REVOLUTIONARY_THRESHOLD) {
                all_revolutionary = false;
                break;
            }
        }

        if (all_revolutionary) {
            std::cout << "  ✅ FULL REVOLUTIONARY STATUS ACHIEVED!\n";
            std::cout << "  🧠 COMPUTATIONAL INTELLIGENCE OPERATIONAL!\n";
        } else {
            std::cout << "  🔧 REVOLUTION IN PROGRESS - OPTIMIZATION NEEDED\n";
        }
    }
};

} // namespace DublinProtocol
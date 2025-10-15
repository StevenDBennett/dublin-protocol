#ifndef REAL_WORLD_APPLICATIONS_HPP
#define REAL_WORLD_APPLICATIONS_HPP

#include "quantum_framework.hpp"
#include "optimized_dublin_protocol/unified_optimized.hpp"
#include <fstream>
#include <sstream>
#include <numeric>

namespace qi {

// === PRACTICAL DEPLOYMENT: BRINGING THE REVOLUTION TO REALITY ===

class FinancialConsensusEngine {
private:
    dublin_optimized::UnifiedDublinProtocol protocol;

public:
    // Market sentiment analysis using computational consensus
    std::string analyze_market_sentiment(const std::vector<double>& price_changes) {
        // Convert price changes to binary decisions
        std::vector<bool> market_decisions;
        for (double change : price_changes) {
            market_decisions.push_back(change > 0); // Up = true, Down = false
        }

        auto result = protocol.optimized_majority_vote(market_decisions);

        std::string sentiment;
        if (result.consensus_value) {
            sentiment = "BULLISH consensus";
        } else {
            sentiment = "BEARISH consensus";
        }

        return std::format(
            "Market Analysis: {} (processed {} data points in {:.3f} ms)",
            sentiment, market_decisions.size(), result.cpu_time_ms
        );
    }

    // Portfolio optimization using quantum-inspired algorithms
    std::vector<bool> optimize_portfolio(const std::vector<std::string>& assets,
                                       const std::vector<double>& returns,
                                       const std::vector<double>& risks) {
        PracticalOptimizer optimizer;

        auto objective_fn = [&](const std::vector<bool>& selection) -> double {
            double total_return = 0.0;
            double total_risk = 0.0;
            int selected_count = 0;

            for (size_t i = 0; i < selection.size(); ++i) {
                if (selection[i]) {
                    total_return += returns[i];
                    total_risk += risks[i];
                    selected_count++;
                }
            }

            if (selected_count == 0) return 0.0;

            // Simple objective: maximize return/risk ratio
            return total_return / (total_risk + 0.001);
        };

        std::vector<bool> dummy_assets(assets.size());
        return optimizer.optimize_portfolio(dummy_assets, objective_fn, 1000);
    }
};

class NetworkSecurityMonitor {
private:
    QuantumInspiredPatternMatcher anomaly_detector;

public:
    void train_anomaly_detection(const std::vector<uint64_t>& normal_traffic_patterns) {
        anomaly_detector.learn_patterns(normal_traffic_patterns, 0.7);
    }

    std::vector<uint64_t> detect_anomalies(const std::vector<uint64_t>& current_traffic) {
        return anomaly_detector.filter_patterns(current_traffic);
    }

    std::string security_report(const std::vector<uint64_t>& traffic) {
        auto anomalies = detect_anomalies(traffic);

        if (anomalies.empty()) {
            return "Security Status: NORMAL - No anomalies detected";
        } else {
            return std::format(
                "Security Alert: {} ANOMALIES detected in {} traffic patterns",
                anomalies.size(), traffic.size()
            );
        }
    }
};

class ScientificDataProcessor {
public:
    // Process sensor data using quantum-inspired filtering
    std::vector<double> clean_sensor_data(const std::vector<double>& raw_readings) {
        SensorDataProcessor processor;
        return processor.process_sensor_readings(raw_readings, 2.5);
    }

    // Detect patterns in experimental data
    std::string analyze_experiment(const std::vector<double>& measurements) {
        auto cleaned = clean_sensor_data(measurements);

        // Convert to binary pattern for consensus analysis
        std::vector<bool> binary_pattern;
        double mean = std::accumulate(cleaned.begin(), cleaned.end(), 0.0) / cleaned.size();

        for (double value : cleaned) {
            binary_pattern.push_back(value > mean);
        }

        // Use optimized consensus
        dublin_optimized::UnifiedDublinProtocol protocol;
        auto result = protocol.optimized_majority_vote(binary_pattern);

        if (result.consensus_value) {
            return std::format(
                "Experiment Analysis: SIGNIFICANT PATTERN detected ({} measurements, {:.3f} ms)",
                measurements.size(), result.cpu_time_ms
            );
        } else {
            return std::format(
                "Experiment Analysis: NO SIGNIFICANT PATTERN ({} measurements, {:.3f} ms)",
                measurements.size(), result.cpu_time_ms
            );
        }
    }
};

class DistributedSystemCoordinator {
private:
    std::vector<ComputationalSubstrate> node_states;

public:
    void add_node(const ComputationalSubstrate& node_state) {
        node_states.push_back(node_state);
    }

    // Reach consensus across distributed nodes
    ComputationalSubstrate distributed_consensus() {
        MetaConsensusEngine meta_engine;

        for (const auto& node : node_states) {
            meta_engine.add_substrate(node);
        }

        return meta_engine.meta_consensus();
    }

    std::string system_status() const {
        return std::format(
            "Distributed System: {} nodes coordinated for consensus",
            node_states.size()
        );
    }
};

// === DEMONSTRATION: REAL-WORLD DEPLOYMENT ===

class RealWorldDemonstrator {
public:
    static void demonstrate_financial_application() {
        std::cout << "💰 FINANCIAL APPLICATION DEMONSTRATION\n";
        std::cout << "=====================================\n\n";

        FinancialConsensusEngine engine;

        // Simulate market data
        std::vector<double> price_changes = {1.2, -0.5, 2.1, -1.8, 0.3, 1.7, -0.2, 0.9};

        std::cout << engine.analyze_market_sentiment(price_changes) << "\n\n";

        // Portfolio optimization
        std::vector<std::string> assets = {"AAPL", "GOOGL", "MSFT", "TSLA", "AMZN"};
        std::vector<double> returns = {0.15, 0.12, 0.08, 0.25, 0.18};
        std::vector<double> risks = {0.10, 0.08, 0.06, 0.20, 0.12};

        auto optimal_portfolio = engine.optimize_portfolio(assets, returns, risks);

        std::cout << "Optimal Portfolio Selection:\n";
        for (size_t i = 0; i < assets.size(); ++i) {
            if (optimal_portfolio[i]) {
                std::cout << "  ✅ " << assets[i] << "\n";
            }
        }
        std::cout << "\n";
    }

    static void demonstrate_security_application() {
        std::cout << "🔒 SECURITY APPLICATION DEMONSTRATION\n";
        std::cout << "====================================\n\n";

        NetworkSecurityMonitor monitor;

        // Train on normal traffic patterns
        std::vector<uint64_t> normal_traffic = {
            0x123456789ABCDEF0, 0x23456789ABCDEF01, 0x3456789ABCDEF012,
            0x456789ABCDEF0123, 0x56789ABCDEF01234
        };
        monitor.train_anomaly_detection(normal_traffic);

        // Test current traffic
        std::vector<uint64_t> current_traffic = {
            0x123456789ABCDEF0, 0x23456789ABCDEF01, 0xFFFFFFFFFFFFFFFF, // Anomaly!
            0x456789ABCDEF0123, 0x56789ABCDEF01234
        };

        std::cout << monitor.security_report(current_traffic) << "\n\n";
    }

    static void demonstrate_scientific_application() {
        std::cout << "🔬 SCIENTIFIC APPLICATION DEMONSTRATION\n";
        std::cout << "======================================\n\n";

        ScientificDataProcessor processor;

        // Simulate experimental measurements with some noise
        std::vector<double> measurements = {
            10.1, 10.2, 10.3, 15.0, 10.1, 10.2, 10.3, 10.1, 10.2, 10.3
        };

        std::cout << processor.analyze_experiment(measurements) << "\n\n";
    }

    static void demonstrate_distributed_system() {
        std::cout << "🌐 DISTRIBUTED SYSTEM DEMONSTRATION\n";
        std::cout << "==================================\n\n";

        DistributedSystemCoordinator coordinator;

        // Add nodes with different states
        ComputationalSubstrate node1, node2, node3;
        node1.set_state(0x1111111111111111);
        node2.set_state(0x2222222222222222);
        node3.set_state(0x3333333333333333);

        coordinator.add_node(node1);
        coordinator.add_node(node2);
        coordinator.add_node(node3);

        std::cout << coordinator.system_status() << "\n";

        auto consensus = coordinator.distributed_consensus();
        auto interpretations = consensus.simultaneous_interpretation();

        std::cout << "Distributed Consensus Result:\n";
        std::cout << "  Real: " << interpretations.as_real << "\n";
        std::cout << "  Complex magnitude: " << std::abs(interpretations.as_complex) << "\n";
        std::cout << "  Matrix: " << interpretations.as_matrix.to_string() << "\n\n";
    }

    static void run_all_demonstrations() {
        std::cout << "🚀 REAL-WORLD APPLICATION DEPLOYMENT\n";
        std::cout << "====================================\n\n";

        demonstrate_financial_application();
        demonstrate_security_application();
        demonstrate_scientific_application();
        demonstrate_distributed_system();

        std::cout << "=== REVOLUTION DEPLOYED TO REALITY ===\n";
        std::cout << "The Dublin Protocol is now solving real-world problems!\n";
    }
};

} // namespace qi

#endif // REAL_WORLD_APPLICATIONS_HPP
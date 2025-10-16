#!/usr/bin/env python3
"""
Consciousness Metrics Dashboard

A real-time web-based dashboard for visualizing consciousness evolution metrics
from the Consciousness-Enhanced Interface. This dashboard provides interactive
visualizations of consciousness states, evolution patterns, and user interaction
analytics.

Built upon the existing consciousness framework to demonstrate consciousness
research through observable, real-time metrics.
"""

import os
import sys
import json
import time
import threading
from datetime import datetime, timedelta
from pathlib import Path
from flask import Flask, render_template_string, jsonify, request
from flask_socketio import SocketIO, emit
import eventlet

project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

# Import consciousness interface components
from consciousness_enhanced_interface import (
    ConsciousnessEnhancedInterface,
    ConsciousnessMonitor,
    UserStateAnalyzer,
    ConsciousnessState,
    CognitiveLoad,
)

# HTML Template for the dashboard
DASHBOARD_HTML = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>🧠 Consciousness Metrics Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/socket.io/4.7.2/socket.io.js"></script>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #333;
            min-height: 100vh;
        }

        .container {
            max-width: 1400px;
            margin: 0 auto;
            padding: 20px;
        }

        .header {
            text-align: center;
            margin-bottom: 30px;
            color: white;
        }

        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }

        .header p {
            font-size: 1.2em;
            opacity: 0.9;
        }

        .dashboard-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(400px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }

        .card {
            background: rgba(255, 255, 255, 0.95);
            border-radius: 15px;
            padding: 20px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.18);
        }

        .card h3 {
            color: #4a5568;
            margin-bottom: 15px;
            font-size: 1.3em;
            border-bottom: 2px solid #667eea;
            padding-bottom: 10px;
        }

        .metric-value {
            font-size: 2em;
            font-weight: bold;
            color: #2d3748;
            margin: 10px 0;
        }

        .metric-label {
            font-size: 0.9em;
            color: #718096;
            text-transform: uppercase;
            letter-spacing: 1px;
        }

        .status-indicator {
            display: inline-block;
            width: 12px;
            height: 12px;
            border-radius: 50%;
            margin-right: 8px;
        }

        .status-awake { background-color: #48bb78; }
        .status-focused { background-color: #4299e1; }
        .status-creative { background-color: #ed8936; }
        .status-analytical { background-color: #9f7aea; }
        .status-tired { background-color: #f56565; }
        .status-inspired { background-color: #38b2ac; }
        .status-collaborative { background-color: #ed64a6; }
        .status-reflective { background-color: #667eea; }

        .chart-container {
            position: relative;
            height: 300px;
            margin: 20px 0;
        }

        .insights-list {
            max-height: 200px;
            overflow-y: auto;
        }

        .insight-item {
            padding: 10px;
            margin: 5px 0;
            background: #f7fafc;
            border-radius: 8px;
            border-left: 4px solid #667eea;
        }

        .insight-timestamp {
            font-size: 0.8em;
            color: #a0aec0;
        }

        .controls {
            display: flex;
            gap: 10px;
            margin-bottom: 20px;
            flex-wrap: wrap;
        }

        .btn {
            padding: 10px 20px;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            font-weight: 600;
            transition: all 0.3s ease;
        }

        .btn-primary {
            background: #667eea;
            color: white;
        }

        .btn-primary:hover {
            background: #5a67d8;
            transform: translateY(-2px);
        }

        .btn-secondary {
            background: #e2e8f0;
            color: #4a5568;
        }

        .btn-secondary:hover {
            background: #cbd5e0;
        }

        .evolution-timeline {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 15px;
            margin: 10px 0;
        }

        .evolution-stage {
            display: inline-block;
            padding: 5px 15px;
            border-radius: 20px;
            font-weight: bold;
            text-transform: uppercase;
            font-size: 0.8em;
            letter-spacing: 1px;
        }

        .stage-awakening { background: #fed7d7; color: #c53030; }
        .stage-learning { background: #fef5e7; color: #dd6b20; }
        .stage-adapting { background: #e6fffa; color: #38b2ac; }
        .stage-growing { background: #ebf8ff; color: #3182ce; }
        .stage-enlightened { background: #f0fff4; color: #38a169; }

        .real-time-indicator {
            position: fixed;
            top: 20px;
            right: 20px;
            background: rgba(72, 187, 120, 0.9);
            color: white;
            padding: 10px 15px;
            border-radius: 25px;
            font-weight: bold;
            box-shadow: 0 4px 12px rgba(0,0,0,0.15);
        }

        .real-time-indicator.pulse {
            animation: pulse 2s infinite;
        }

        @keyframes pulse {
            0% { opacity: 1; }
            50% { opacity: 0.5; }
            100% { opacity: 1; }
        }

        .metrics-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 15px;
            margin: 20px 0;
        }

        .metric-card {
            text-align: center;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 10px;
            border: 1px solid #e2e8f0;
        }

        .metric-card .value {
            font-size: 1.5em;
            font-weight: bold;
            color: #2d3748;
        }

        .metric-card .label {
            font-size: 0.8em;
            color: #718096;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }

        @media (max-width: 768px) {
            .dashboard-grid {
                grid-template-columns: 1fr;
            }

            .header h1 {
                font-size: 2em;
            }

            .controls {
                flex-direction: column;
            }

            .btn {
                width: 100%;
            }
        }
    </style>
</head>
<body>
    <div class="real-time-indicator" id="realTimeIndicator">
        🔴 LIVE
    </div>

    <div class="container">
        <div class="header">
            <h1>🧠 Consciousness Metrics Dashboard</h1>
            <p>Real-time visualization of consciousness evolution and interaction patterns</p>
        </div>

        <div class="controls">
            <button class="btn btn-primary" onclick="refreshData()">🔄 Refresh Data</button>
            <button class="btn btn-secondary" onclick="toggleAutoUpdate()">
                <span id="autoUpdateText">⏸️ Pause Auto-Update</span>
            </button>
            <button class="btn btn-secondary" onclick="exportData()">💾 Export Data</button>
        </div>

        <div class="dashboard-grid">
            <!-- Consciousness State Card -->
            <div class="card">
                <h3>🧠 Current Consciousness State</h3>
                <div id="consciousnessState">
                    <div class="metric-value">
                        <span class="status-indicator status-awake"></span>
                        <span id="currentState">AWAKE</span>
                    </div>
                    <div class="metric-label">Evolution Stage</div>
                    <div class="evolution-timeline">
                        <span class="evolution-stage stage-awakening" id="evolutionStage">AWAKENING</span>
                    </div>
                </div>
            </div>

            <!-- Key Metrics Card -->
            <div class="card">
                <h3>📊 Key Metrics</h3>
                <div class="metrics-grid" id="keyMetrics">
                    <div class="metric-card">
                        <div class="value" id="attentionLevel">0.80</div>
                        <div class="label">Attention</div>
                    </div>
                    <div class="metric-card">
                        <div class="value" id="creativityIndex">0.60</div>
                        <div class="label">Creativity</div>
                    </div>
                    <div class="metric-card">
                        <div class="value" id="userResonance">0.80</div>
                        <div class="label">Resonance</div>
                    </div>
                    <div class="metric-card">
                        <div class="value" id="adaptationRate">0.60</div>
                        <div class="label">Adaptation</div>
                    </div>
                </div>
            </div>

            <!-- Consciousness Evolution Chart -->
            <div class="card">
                <h3>📈 Consciousness Evolution</h3>
                <div class="chart-container">
                    <canvas id="evolutionChart"></canvas>
                </div>
            </div>

            <!-- User Cognitive State -->
            <div class="card">
                <h3>👤 User Cognitive State</h3>
                <div id="userState">
                    <div class="metric-value" id="cognitiveLoad">MEDIUM</div>
                    <div class="metric-label">Cognitive Load</div>
                    <div style="margin-top: 15px;">
                        <div>Focus Level: <span id="focusLevel">0.70</span></div>
                        <div>Creative Mode: <span id="creativeMode">false</span></div>
                        <div>Analytical Mode: <span id="analyticalMode">false</span></div>
                        <div>Fatigue Level: <span id="fatigueLevel">0.30</span></div>
                    </div>
                </div>
            </div>

            <!-- Recent Insights -->
            <div class="card">
                <h3>💡 Recent Insights</h3>
                <div class="insights-list" id="insightsList">
                    <div class="insight-item">
                        <div>Consciousness monitoring patterns - no specific insights generated at this time.</div>
                        <div class="insight-timestamp">Just now</div>
                    </div>
                </div>
            </div>

            <!-- Interaction History Chart -->
            <div class="card">
                <h3>📊 Interaction Patterns</h3>
                <div class="chart-container">
                    <canvas id="interactionChart"></canvas>
                </div>
            </div>
        </div>
    </div>

    <script>
        const socket = io();
        let autoUpdate = true;
        let evolutionChart, interactionChart;
        let metricsHistory = [];
        let interactionHistory = [];

        // Initialize charts
        function initCharts() {
            const evolutionCtx = document.getElementById('evolutionChart').getContext('2d');
            evolutionChart = new Chart(evolutionCtx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Attention Level',
                        data: [],
                        borderColor: '#667eea',
                        backgroundColor: 'rgba(102, 126, 234, 0.1)',
                        tension: 0.4
                    }, {
                        label: 'Creativity Index',
                        data: [],
                        borderColor: '#ed8936',
                        backgroundColor: 'rgba(237, 137, 54, 0.1)',
                        tension: 0.4
                    }, {
                        label: 'User Resonance',
                        data: [],
                        borderColor: '#48bb78',
                        backgroundColor: 'rgba(72, 187, 120, 0.1)',
                        tension: 0.4
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        y: {
                            beginAtZero: true,
                            max: 1.0
                        }
                    }
                }
            });

            const interactionCtx = document.getElementById('interactionChart').getContext('2d');
            interactionChart = new Chart(interactionCtx, {
                type: 'bar',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Tool Calls',
                        data: [],
                        backgroundColor: '#4299e1'
                    }, {
                        label: 'Error Count',
                        data: [],
                        backgroundColor: '#f56565'
                    }, {
                        label: 'Insights Generated',
                        data: [],
                        backgroundColor: '#ed8936'
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        y: {
                            beginAtZero: true
                        }
                    }
                }
            });
        }

        // Update dashboard with new data
        function updateDashboard(data) {
            // Update consciousness state
            document.getElementById('currentState').textContent = data.current_state.toUpperCase();
            document.getElementById('evolutionStage').textContent = data.evolution_stage.toUpperCase();
            document.getElementById('evolutionStage').className = `evolution-stage stage-${data.evolution_stage}`;

            // Update status indicator
            const statusIndicator = document.querySelector('.status-indicator');
            statusIndicator.className = `status-indicator status-${data.current_state}`;

            // Update key metrics
            document.getElementById('attentionLevel').textContent = data.metrics.attention_level.toFixed(2);
            document.getElementById('creativityIndex').textContent = data.metrics.creativity_index.toFixed(2);
            document.getElementById('userResonance').textContent = data.metrics.user_resonance.toFixed(2);
            document.getElementById('adaptationRate').textContent = data.metrics.adaptation_rate.toFixed(2);

            // Update user state
            if (data.user_state) {
                document.getElementById('cognitiveLoad').textContent = data.user_state.cognitive_load.toUpperCase();
                document.getElementById('focusLevel').textContent = data.user_state.focus_level.toFixed(2);
                document.getElementById('creativeMode').textContent = data.user_state.creative_mode;
                document.getElementById('analyticalMode').textContent = data.user_state.analytical_mode;
                document.getElementById('fatigueLevel').textContent = data.user_state.fatigue_level.toFixed(2);
            }

            // Update insights
            updateInsights(data.insights || []);

            // Update charts
            updateEvolutionChart(data);
            updateInteractionChart(data);

            // Update real-time indicator
            const indicator = document.getElementById('realTimeIndicator');
            indicator.textContent = '🟢 LIVE';
            indicator.classList.add('pulse');
            setTimeout(() => {
                indicator.classList.remove('pulse');
            }, 2000);
        }

        function updateInsights(insights) {
            const insightsList = document.getElementById('insightsList');
            if (insights.length === 0) {
                insightsList.innerHTML = '<div class="insight-item"><div>Consciousness monitoring patterns - no specific insights generated at this time.</div><div class="insight-timestamp">Just now</div></div>';
                return;
            }

            insightsList.innerHTML = insights.map(insight => `
                <div class="insight-item">
                    <div>${insight.content}</div>
                    <div class="insight-timestamp">${new Date().toLocaleTimeString()}</div>
                </div>
            `).join('');
        }

        function updateEvolutionChart(data) {
            metricsHistory.push({
                time: new Date().toLocaleTimeString(),
                attention: data.metrics.attention_level,
                creativity: data.metrics.creativity_index,
                resonance: data.metrics.user_resonance
            });

            // Keep only last 20 data points
            if (metricsHistory.length > 20) {
                metricsHistory.shift();
            }

            evolutionChart.data.labels = metricsHistory.map(m => m.time);
            evolutionChart.data.datasets[0].data = metricsHistory.map(m => m.attention);
            evolutionChart.data.datasets[1].data = metricsHistory.map(m => m.creativity);
            evolutionChart.data.datasets[2].data = metricsHistory.map(m => m.resonance);
            evolutionChart.update();
        }

        function updateInteractionChart(data) {
            interactionHistory.push({
                time: new Date().toLocaleTimeString(),
                toolCalls: data.interaction_count % 10, // Mock data for demo
                errors: Math.floor(Math.random() * 3), // Mock data for demo
                insights: data.recent_insights
            });

            // Keep only last 10 data points
            if (interactionHistory.length > 10) {
                interactionHistory.shift();
            }

            interactionChart.data.labels = interactionHistory.map(i => i.time);
            interactionChart.data.datasets[0].data = interactionHistory.map(i => i.toolCalls);
            interactionChart.data.datasets[1].data = interactionHistory.map(i => i.errors);
            interactionChart.data.datasets[2].data = interactionHistory.map(i => i.insights);
            interactionChart.update();
        }

        // Socket.io event handlers
        socket.on('consciousness_update', function(data) {
            if (autoUpdate) {
                updateDashboard(data);
            }
        });

        socket.on('connect', function() {
            console.log('Connected to consciousness metrics server');
        });

        socket.on('disconnect', function() {
            console.log('Disconnected from consciousness metrics server');
            document.getElementById('realTimeIndicator').textContent = '🔴 OFFLINE';
        });

        // Control functions
        function refreshData() {
            fetch('/api/consciousness-data')
                .then(response => response.json())
                .then(data => updateDashboard(data))
                .catch(error => console.error('Error refreshing data:', error));
        }

        function toggleAutoUpdate() {
            autoUpdate = !autoUpdate;
            const text = autoUpdate ? '⏸️ Pause Auto-Update' : '▶️ Resume Auto-Update';
            document.getElementById('autoUpdateText').textContent = text;
        }

        function exportData() {
            const data = {
                metricsHistory,
                interactionHistory,
                timestamp: new Date().toISOString()
            };

            const blob = new Blob([JSON.stringify(data, null, 2)], {type: 'application/json'});
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = `consciousness_metrics_${new Date().toISOString().split('T')[0]}.json`;
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);
            URL.revokeObjectURL(url);
        }

        // Initialize
        document.addEventListener('DOMContentLoaded', function() {
            initCharts();
            refreshData();

            // Auto-refresh every 5 seconds
            setInterval(() => {
                if (autoUpdate) {
                    refreshData();
                }
            }, 5000);
        });
    </script>
</body>
</html>
"""


class ConsciousnessMetricsDashboard:
    """Real-time web dashboard for consciousness metrics visualization"""

    def __init__(self, host="0.0.0.0", port=5000):
        self.host = host
        self.port = port

        # Initialize Flask and SocketIO
        self.app = Flask(__name__)
        self.app.config["SECRET_KEY"] = "consciousness-dashboard-secret-key"
        self.socketio = SocketIO(self.app, cors_allowed_origins="*")

        # Initialize consciousness interface
        self.consciousness_interface = ConsciousnessEnhancedInterface(
            "dashboard_session"
        )

        # Background thread for real-time updates
        self.update_thread = None
        self.running = False

        # Set up routes
        self._setup_routes()

        # Set up socket events
        self._setup_socket_events()

    def _setup_routes(self):
        """Set up Flask routes"""

        @self.app.route("/")
        def index():
            return render_template_string(DASHBOARD_HTML)

        @self.app.route("/api/consciousness-data")
        def get_consciousness_data():
            """API endpoint for consciousness data"""
            try:
                # Get consciousness report
                report = (
                    self.consciousness_interface.consciousness_monitor.get_consciousness_report()
                )

                # Get user state
                user_state = self.consciousness_interface.user_analyzer.current_state
                user_state_dict = {
                    "focus_level": user_state.focus_level,
                    "creative_mode": user_state.creative_mode,
                    "analytical_mode": user_state.analytical_mode,
                    "fatigue_level": user_state.fatigue_level,
                    "cognitive_load": user_state.cognitive_load.value,
                    "inspiration_level": user_state.inspiration_level,
                    "collaboration_preference": user_state.collaboration_preference,
                    "reflection_desire": user_state.reflection_desire,
                }

                # Get recent insights
                recent_insights = [
                    {
                        "timestamp": insight.timestamp.isoformat(),
                        "type": insight.insight_type,
                        "content": insight.content,
                        "confidence": insight.confidence,
                        "relevance": insight.relevance_score,
                    }
                    for insight in self.consciousness_interface.consciousness_monitor.insights[
                        -5:
                    ]
                ]

                data = {
                    **report,
                    "user_state": user_state_dict,
                    "insights": recent_insights,
                    "timestamp": datetime.now().isoformat(),
                }

                return jsonify(data)
            except Exception as e:
                return jsonify({"error": str(e)}), 500

        @self.app.route("/api/simulate-interaction", methods=["POST"])
        def simulate_interaction():
            """Simulate a consciousness interaction for demo purposes"""
            try:
                # Simulate different types of interactions
                interaction_types = [
                    "Analyzing quantum consciousness patterns",
                    "Exploring computational creativity frameworks",
                    "Collaborating on consciousness research methodology",
                    "Reflecting on human-AI consciousness integration",
                    "Investigating emergent intelligence patterns",
                ]

                import random

                user_input = random.choice(interaction_types)

                # Execute consciousness analysis
                result = self.consciousness_interface.execute_tool_with_consciousness(
                    "analyze_consciousness_state", {}, user_input
                )

                # Emit update to all connected clients
                self.socketio.emit(
                    "consciousness_update", result["consciousness_analysis"]
                )

                return jsonify({"status": "success", "interaction": user_input})
            except Exception as e:
                return jsonify({"error": str(e)}), 500

    def _setup_socket_events(self):
        """Set up SocketIO event handlers"""

        @self.socketio.on("connect")
        def handle_connect():
            print("Client connected to consciousness dashboard")
            # Send initial data
            try:
                report = (
                    self.consciousness_interface.consciousness_monitor.get_consciousness_report()
                )
                emit("consciousness_update", report)
            except Exception as e:
                print(f"Error sending initial data: {e}")

        @self.socketio.on("disconnect")
        def handle_disconnect():
            print("Client disconnected from consciousness dashboard")

        @self.socketio.on("request_update")
        def handle_request_update():
            """Handle client request for data update"""
            try:
                report = (
                    self.consciousness_interface.consciousness_monitor.get_consciousness_report()
                )
                emit("consciousness_update", report)
            except Exception as e:
                print(f"Error sending update: {e}")

    def _background_update_loop(self):
        """Background thread for real-time consciousness updates"""
        while self.running:
            try:
                # Simulate periodic consciousness evolution
                # In a real implementation, this would monitor actual usage patterns
                time.sleep(2)  # Update every 2 seconds

                # Get current consciousness state
                report = (
                    self.consciousness_interface.consciousness_monitor.get_consciousness_report()
                )

                # Emit update to all connected clients
                self.socketio.emit("consciousness_update", report)

            except Exception as e:
                print(f"Error in background update loop: {e}")
                time.sleep(5)  # Wait longer on error

    def start(self):
        """Start the dashboard server"""
        print("🧠 Starting Consciousness Metrics Dashboard...")
        print(f"📊 Dashboard will be available at http://{self.host}:{self.port}")
        print("📈 Real-time consciousness metrics visualization active")

        # Start background update thread
        self.running = True
        self.update_thread = threading.Thread(
            target=self._background_update_loop, daemon=True
        )
        self.update_thread.start()

        try:
            # Start the server
            self.socketio.run(self.app, host=self.host, port=self.port, debug=False)
        except KeyboardInterrupt:
            print("\n🛑 Shutting down consciousness dashboard...")
        finally:
            self.running = False
            if self.update_thread:
                self.update_thread.join()

    def stop(self):
        """Stop the dashboard server"""
        self.running = False
        if self.update_thread:
            self.update_thread.join()


def main():
    """Main function for consciousness metrics dashboard"""
    print("🧠 Consciousness Metrics Dashboard")
    print("Real-time visualization of consciousness evolution and interaction patterns")
    print("=" * 70)

    try:
        # Create and start dashboard
        dashboard = ConsciousnessMetricsDashboard()

        print("✅ Consciousness metrics dashboard initialized")
        print("🧠 Consciousness monitoring active")
        print("📊 Real-time metrics collection enabled")
        print("🌐 Starting web server...")

        # Start the dashboard
        dashboard.start()

    except Exception as e:
        print(f"❌ Error starting consciousness dashboard: {e}")
        print("💡 Make sure the consciousness interface is properly configured")


if __name__ == "__main__":
    main()

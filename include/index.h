#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CAN Bus Monitor</title>
    <script src="/chart.js"></script>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            color: #333;
        }
        
        .container {
            max-width: 1400px;
            margin: 0 auto;
            padding: 20px;
        }
        
        .header {
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(10px);
            border-radius: 15px;
            padding: 20px;
            margin-bottom: 20px;
            box-shadow: 0 8px 32px rgba(31, 38, 135, 0.37);
            border: 1px solid rgba(255, 255, 255, 0.18);
        }
        
        .header h1 {
            text-align: center;
            color: #4a5568;
            margin-bottom: 15px;
            font-size: 2.5em;
            font-weight: 300;
        }
        
        .connection-status {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
            margin-bottom: 20px;
        }
        
        .status-indicator {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            background: #ef4444;
            transition: all 0.3s ease;
        }
        
        .status-indicator.connected {
            background: #10b981;
            box-shadow: 0 0 10px rgba(16, 185, 129, 0.5);
        }
        
        .controls {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 20px;
            flex-wrap: wrap;
        }
        
        .control-group {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px;
        }
        
        .control-group label {
            font-weight: 600;
            color: #4a5568;
            font-size: 0.9em;
        }
        
        select, button {
            padding: 12px 20px;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            cursor: pointer;
            transition: all 0.3s ease;
        }
        
        select {
            background: white;
            border: 2px solid #e2e8f0;
            color: #4a5568;
            min-width: 150px;
        }
        
        select:focus {
            outline: none;
            border-color: #667eea;
            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
        }
        
        button {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            font-weight: 600;
            transform: translateY(0);
        }
        
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(102, 126, 234, 0.4);
        }
        
        button:disabled {
            background: #cbd5e0;
            cursor: not-allowed;
            transform: none;
        }
        
        .main-content {
            display: grid;
            grid-template-columns: 1fr 300px;
            gap: 20px;
            margin-bottom: 20px;
        }
        
        .chart-container {
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(10px);
            border-radius: 15px;
            padding: 20px;
            box-shadow: 0 8px 32px rgba(31, 38, 135, 0.37);
            border: 1px solid rgba(255, 255, 255, 0.18);
        }
        
        .sidebar {
            display: flex;
            flex-direction: column;
            gap: 15px;
        }
        
        .info-panel {
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(10px);
            border-radius: 15px;
            padding: 20px;
            box-shadow: 0 8px 32px rgba(31, 38, 135, 0.37);
            border: 1px solid rgba(255, 255, 255, 0.18);
        }
        
        .info-panel h3 {
            color: #4a5568;
            margin-bottom: 15px;
            font-size: 1.2em;
            font-weight: 600;
        }
        
        .byte-values {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 10px;
        }
        
        .byte-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 8px 12px;
            background: #f7fafc;
            border-radius: 6px;
            border-left: 3px solid #667eea;
        }
        
        .byte-label {
            font-weight: 600;
            color: #4a5568;
            font-size: 0.9em;
        }
        
        .byte-value {
            font-family: 'Courier New', monospace;
            font-weight: bold;
            color: #2d3748;
        }
        
        .stats-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
            margin-top: 15px;
        }
        
        .stat-item {
            text-align: center;
            padding: 15px;
            background: #f7fafc;
            border-radius: 8px;
            border-top: 3px solid #667eea;
        }
        
        .stat-value {
            font-size: 1.8em;
            font-weight: bold;
            color: #2d3748;
            margin-bottom: 5px;
        }
        
        .stat-label {
            font-size: 0.9em;
            color: #718096;
            font-weight: 500;
        }
        
        .can-ids-list {
            max-height: 300px;
            overflow-y: auto;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            background: #f7fafc;
        }
        
        .can-id-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 10px 15px;
            border-bottom: 1px solid #e2e8f0;
            transition: background 0.2s ease;
        }
        
        .can-id-item:hover {
            background: #edf2f7;
        }
        
        .can-id-item:last-child {
            border-bottom: none;
        }
        
        .can-id-name {
            font-family: 'Courier New', monospace;
            font-weight: bold;
            color: #2d3748;
        }
        
        .can-id-status {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background: #cbd5e0;
        }
        
        .can-id-status.active {
            background: #10b981;
            box-shadow: 0 0 6px rgba(16, 185, 129, 0.5);
        }
        
        @media (max-width: 1024px) {
            .main-content {
                grid-template-columns: 1fr;
            }
            
            .controls {
                flex-direction: column;
                gap: 15px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>CAN Bus Monitor</h1>
            <div class="connection-status">
                <div class="status-indicator" id="connectionStatus"></div>
                <span id="connectionText">Disconnected</span>
            </div>
            <div class="controls">
                <div class="control-group">
                    <label for="canIdSelect">Select CAN ID:</label>
                    <select id="canIdSelect" disabled>
                        <option value="">Loading...</option>
                    </select>
                </div>
                <div class="control-group">
                    <label>&nbsp;</label>
                    <button id="startBtn" disabled>Start Monitoring</button>
                </div>
                <div class="control-group">
                    <label>&nbsp;</label>
                    <button id="clearBtn">Clear Chart</button>
                </div>
            </div>
        </div>

        <div class="main-content">
            <div class="chart-container">
                <canvas id="byteChart"></canvas>
            </div>
            
            <div class="sidebar">
                <div class="info-panel">
                    <h3>Current Values</h3>
                    <div class="byte-values" id="byteValues">
                        <!-- Byte values will be populated here -->
                    </div>
                </div>
                
                <div class="info-panel">
                    <h3>Statistics</h3>
                    <div class="stats-grid">
                        <div class="stat-item">
                            <div class="stat-value" id="frameCount">0</div>
                            <div class="stat-label">Frames</div>
                        </div>
                        <div class="stat-item">
                            <div class="stat-value" id="frameRate">0</div>
                            <div class="stat-label">FPS</div>
                        </div>
                    </div>
                </div>
                
                <div class="info-panel">
                    <h3>Available CAN IDs</h3>
                    <div class="can-ids-list" id="canIdsList">
                        <!-- CAN IDs will be populated here -->
                    </div>
                </div>
            </div>
        </div>
    </div>

    <script>
        class CANMonitor {
            constructor() {
                this.ws = null;
                this.chart = null;
                this.isMonitoring = false;
                this.frameCount = 0;
                this.lastFrameTime = Date.now();
                this.frameRate = 0;
                this.availableCANIDs = [];
                this.selectedCANID = null;
                
                this.initChart();
                this.initEventListeners();
                this.connect();
            }
            
            connect() {
                const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
                const wsUrl = `${protocol}//${window.location.host}/ws`;
                
                this.ws = new WebSocket(wsUrl);
                
                this.ws.onopen = () => {
                    this.updateConnectionStatus(true);
                    console.log('WebSocket connected');
                };
                
                this.ws.onclose = () => {
                    this.updateConnectionStatus(false);
                    console.log('WebSocket disconnected');
                    // Attempt to reconnect after 3 seconds
                    setTimeout(() => this.connect(), 3000);
                };
                
                this.ws.onerror = (error) => {
                    console.error('WebSocket error:', error);
                    this.updateConnectionStatus(false);
                };
                
                this.ws.onmessage = (event) => {
                    try {
                        const data = JSON.parse(event.data);
                        this.handleMessage(data);
                    } catch (error) {
                        console.error('Error parsing WebSocket message:', error);
                    }
                };
            }
            
            updateConnectionStatus(connected) {
                const statusIndicator = document.getElementById('connectionStatus');
                const statusText = document.getElementById('connectionText');
                const canIdSelect = document.getElementById('canIdSelect');
                const startBtn = document.getElementById('startBtn');
                
                if (connected) {
                    statusIndicator.classList.add('connected');
                    statusText.textContent = 'Connected';
                    canIdSelect.disabled = false;
                    startBtn.disabled = false;
                } else {
                    statusIndicator.classList.remove('connected');
                    statusText.textContent = 'Disconnected';
                    canIdSelect.disabled = true;
                    startBtn.disabled = true;
                }
            }
            
            handleMessage(data) {
                switch (data.type) {
                    case 'availableCANIDs':
                        this.updateAvailableCANIDs(data.canIDs);
                        break;
                    case 'byteData':
                        if (this.isMonitoring) {
                            this.updateChart(data);
                            this.updateByteValues(data.bytes);
                            this.updateStats();
                        }
                        break;
                    case 'statusUpdate':
                        this.updateCANIDStatus(data.frameStats);
                        break;
                    case 'canIDSelected':
                        console.log('CAN ID selected:', data.canID);
                        break;
                }
            }
            
            updateAvailableCANIDs(canIDs) {
                this.availableCANIDs = canIDs;
                const select = document.getElementById('canIdSelect');
                const list = document.getElementById('canIdsList');
                
                // Update select dropdown
                select.innerHTML = '<option value="">Select CAN ID...</option>';
                canIDs.forEach(id => {
                    const option = document.createElement('option');
                    option.value = id;
                    option.textContent = id;
                    select.appendChild(option);
                });
                
                // Update CAN IDs list
                list.innerHTML = '';
                canIDs.forEach(id => {
                    const item = document.createElement('div');
                    item.className = 'can-id-item';
                    item.innerHTML = `
                        <span class="can-id-name">${id}</span>
                        <div class="can-id-status" id="status-${id}"></div>
                    `;
                    list.appendChild(item);
                });
            }
            
            updateCANIDStatus(frameStats) {
                Object.keys(frameStats).forEach(canID => {
                    const statusElement = document.getElementById(`status-${canID}`);
                    if (statusElement) {
                        const stat = frameStats[canID];
                        const isActive = (Date.now() - stat.lastSeen) < 2000; // Active if seen in last 2 seconds
                        statusElement.classList.toggle('active', isActive);
                    }
                });
            }
            
            initChart() {
                const ctx = document.getElementById('byteChart').getContext('2d');
                
                this.chart = new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels: [],
                        datasets: [
                            { label: 'Byte 0', data: [], borderColor: '#ff6384', backgroundColor: 'rgba(255, 99, 132, 0.1)', tension: 0.4 },
                            { label: 'Byte 1', data: [], borderColor: '#36a2eb', backgroundColor: 'rgba(54, 162, 235, 0.1)', tension: 0.4 },
                            { label: 'Byte 2', data: [], borderColor: '#cc65fe', backgroundColor: 'rgba(204, 101, 254, 0.1)', tension: 0.4 },
                            { label: 'Byte 3', data: [], borderColor: '#ffce56', backgroundColor: 'rgba(255, 206, 86, 0.1)', tension: 0.4 },
                            { label: 'Byte 4', data: [], borderColor: '#4bc0c0', backgroundColor: 'rgba(75, 192, 192, 0.1)', tension: 0.4 },
                            { label: 'Byte 5', data: [], borderColor: '#9966ff', backgroundColor: 'rgba(153, 102, 255, 0.1)', tension: 0.4 },
                            { label: 'Byte 6', data: [], borderColor: '#ff9f40', backgroundColor: 'rgba(255, 159, 64, 0.1)', tension: 0.4 },
                            { label: 'Byte 7', data: [], borderColor: '#ff6384', backgroundColor: 'rgba(255, 99, 132, 0.1)', tension: 0.4 }
                        ]
                    },
                    options: {
                        responsive: true,
                        maintainAspectRatio: false,
                        scales: {
                            x: {
                                type: 'linear',
                                position: 'bottom',
                                title: {
                                    display: true,
                                    text: 'Time (ms)'
                                }
                            },
                            y: {
                                beginAtZero: true,
                                max: 255,
                                title: {
                                    display: true,
                                    text: 'Byte Value'
                                }
                            }
                        },
                        plugins: {
                            title: {
                                display: true,
                                text: 'CAN Frame Bytes (Live)',
                                font: {
                                    size: 16,
                                    weight: 'bold'
                                }
                            },
                            legend: {
                                position: 'top'
                            }
                        },
                        animation: {
                            duration: 0
                        },
                        interaction: {
                            intersect: false,
                            mode: 'index'
                        }
                    }
                });
                
                // Set chart container height
                document.getElementById('byteChart').style.height = '400px';
            }
            
            updateChart(data) {
                const timestamp = Date.now();
                const maxDataPoints = 100;
                
                // Add new data point
                this.chart.data.labels.push(timestamp);
                
                data.bytes.forEach((byteValue, index) => {
                    this.chart.data.datasets[index].data.push({
                        x: timestamp,
                        y: byteValue
                    });
                });
                
                // Remove old data points
                if (this.chart.data.labels.length > maxDataPoints) {
                    this.chart.data.labels.shift();
                    this.chart.data.datasets.forEach(dataset => {
                        dataset.data.shift();
                    });
                }
                
                this.chart.update('none');
            }
            
            updateByteValues(bytes) {
                const container = document.getElementById('byteValues');
                container.innerHTML = '';
                
                bytes.forEach((value, index) => {
                    const item = document.createElement('div');
                    item.className = 'byte-item';
                    item.innerHTML = `
                        <span class="byte-label">Byte ${index}</span>
                        <span class="byte-value">${value} (0x${value.toString(16).padStart(2, '0').toUpperCase()})</span>
                    `;
                    container.appendChild(item);
                });
            }
            
            updateStats() {
                this.frameCount++;
                
                const now = Date.now();
                const timeDiff = now - this.lastFrameTime;
                
                if (timeDiff >= 1000) {
                    this.frameRate = Math.round((this.frameCount * 1000) / timeDiff);
                    this.frameCount = 0;
                    this.lastFrameTime = now;
                }
                
                document.getElementById('frameCount').textContent = this.frameCount;
                document.getElementById('frameRate').textContent = this.frameRate;
            }
            
            initEventListeners() {
                const canIdSelect = document.getElementById('canIdSelect');
                const startBtn = document.getElementById('startBtn');
                const clearBtn = document.getElementById('clearBtn');
                
                canIdSelect.addEventListener('change', (e) => {
                    this.selectedCANID = e.target.value;
                    startBtn.disabled = !this.selectedCANID || !this.ws;
                });
                
                startBtn.addEventListener('click', () => {
                    if (!this.isMonitoring && this.selectedCANID) {
                        this.startMonitoring();
                    } else {
                        this.stopMonitoring();
                    }
                });
                
                clearBtn.addEventListener('click', () => {
                    this.clearChart();
                });
            }
            
            startMonitoring() {
                if (!this.selectedCANID || !this.ws) return;
                
                const message = JSON.stringify({
                    action: 'selectCANID',
                    canID: this.selectedCANID
                });
                
                this.ws.send(message);
                this.isMonitoring = true;
                this.frameCount = 0;
                this.lastFrameTime = Date.now();
                
                const startBtn = document.getElementById('startBtn');
                startBtn.textContent = 'Stop Monitoring';
                startBtn.style.background = 'linear-gradient(135deg, #ef4444 0%, #dc2626 100%)';
                
                console.log('Started monitoring CAN ID:', this.selectedCANID);
            }
            
            stopMonitoring() {
                this.isMonitoring = false;
                
                const startBtn = document.getElementById('startBtn');
                startBtn.textContent = 'Start Monitoring';
                startBtn.style.background = 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)';
                
                console.log('Stopped monitoring');
            }
            
            clearChart() {
                this.chart.data.labels = [];
                this.chart.data.datasets.forEach(dataset => {
                    dataset.data = [];
                });
                this.chart.update();
                
                this.frameCount = 0;
                this.frameRate = 0;
                document.getElementById('frameCount').textContent = '0';
                document.getElementById('frameRate').textContent = '0';
                
                console.log('Chart cleared');
            }
        }
        
        // Initialize the CAN monitor when the page loads
        document.addEventListener('DOMContentLoaded', () => {
            new CANMonitor();
        });
    </script>
</body>
</html>
)rawliteral";

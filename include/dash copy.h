#include <Arduino.h>

const char dash_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Vehicle Dashboard</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: Arial, sans-serif;
            background: #0a0a0a;
            color: #fff;
            min-height: 100vh;
            padding: 10px;
        }
        
        .header {
            text-align: center;
            margin-bottom: 20px;
        }
        
        .header h1 {
            font-size: 2em;
            color: #00ff88;
            margin-bottom: 10px;
        }
        
        .status {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
            margin-bottom: 20px;
        }
        
        .status-dot {
            width: 10px;
            height: 10px;
            border-radius: 50%;
            background: #ff4444;
        }
        
        .status-dot.connected {
            background: #00ff88;
        }
        
        .gauges {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-bottom: 30px;
        }
        
        .gauge.large {
            grid-column: span 2;
        }
        
        .gauge.large .gauge-circle {
            width: 180px;
            height: 180px;
        }
        
        .gauge.large .gauge-fill {
            width: 180px;
            height: 180px;
        }
        
        .gauge.large .gauge-value {
            font-size: 2.2em;
        }
        
        .gauge.large .gauge-title {
            font-size: 1.2em;
            margin-bottom: 20px;
        }
        
        .gauge {
            background: #1a1a1a;
            border: 2px solid #333;
            border-radius: 15px;
            padding: 20px;
            text-align: center;
            position: relative;
        }
        
        .gauge-title {
            font-size: 0.9em;
            color: #aaa;
            margin-bottom: 15px;
            text-transform: uppercase;
        }
        
        .gauge-circle {
            width: 120px;
            height: 120px;
            border: 8px solid #333;
            border-radius: 50%;
            margin: 0 auto 15px;
            position: relative;
            background: #0a0a0a;
        }
        
        .gauge-fill {
            position: absolute;
            top: -8px;
            left: -8px;
            width: 120px;
            height: 120px;
            border-radius: 50%;
            border: 8px solid transparent;
            transform: rotate(-90deg);
            transition: all 0.3s ease;
        }
        
        .gauge-fill.speed {
            border-top-color: #00ff88;
            border-right-color: #00ff88;
        }
        
        .gauge-fill.rpm {
            border-top-color: #ff6b35;
            border-right-color: #ff6b35;
        }
        
        .gauge-fill.throttle {
            border-top-color: #4ecdc4;
            border-right-color: #4ecdc4;
        }
        
        .gauge-fill.fuel {
            border-top-color: #ffe66d;
            border-right-color: #ffe66d;
        }
        
        .gauge-fill.temp {
            border-top-color: #ff4757;
            border-right-color: #ff4757;
        }
        
        .gauge-value {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            font-size: 1.5em;
            font-weight: bold;
        }
        
        .gauge-unit {
            font-size: 0.8em;
            color: #aaa;
            margin-top: 5px;
        }
        
        .gauge-range {
            font-size: 0.7em;
            color: #666;
            margin-top: 10px;
        }
        
        .expert-btn {
            display: block;
            width: 200px;
            margin: 0 auto;
            padding: 15px 30px;
            background: #333;
            color: #fff;
            text-decoration: none;
            border-radius: 8px;
            text-align: center;
            font-weight: bold;
            border: 2px solid #555;
            transition: all 0.3s ease;
        }
        
        .expert-btn:hover {
            background: #555;
            border-color: #00ff88;
            color: #00ff88;
        }
        
        @media (max-width: 768px) {
            .gauges {
                grid-template-columns: 1fr;
            }
            
            .gauge.large {
                grid-column: span 1;
            }
            
            .gauge.large .gauge-circle {
                width: 150px;
                height: 150px;
            }
            
            .gauge.large .gauge-fill {
                width: 150px;
                height: 150px;
            }
            
            .gauge-circle {
                width: 100px;
                height: 100px;
            }
            
            .gauge-fill {
                width: 100px;
                height: 100px;
            }
            
            .gauge-value {
                font-size: 1.2em;
            }
            
            .gauge.large .gauge-value {
                font-size: 1.8em;
            }
        }
    </style>
</head>
<body>
    <div class="header">
        <h1>Vehicle Dashboard</h1>
        <div class="status">
            <div class="status-dot" id="statusDot"></div>
            <span id="statusText">Disconnected</span>
        </div>
    </div>

    <div class="gauges">
        <div class="gauge large">
            <div class="gauge-title">Speed</div>
            <div class="gauge-circle">
                <div class="gauge-fill speed" id="speedFill"></div>
                <div class="gauge-value">
                    <span id="speedValue">0</span>
                    <div class="gauge-unit">km/h</div>
                </div>
            </div>
            <div class="gauge-range">0 - 200 km/h</div>
        </div>

        <div class="gauge large">
            <div class="gauge-title">RPM</div>
            <div class="gauge-circle">
                <div class="gauge-fill rpm" id="rpmFill"></div>
                <div class="gauge-value">
                    <span id="rpmValue">0</span>
                    <div class="gauge-unit">RPM</div>
                </div>
            </div>
            <div class="gauge-range">0 - 8000 RPM</div>
        </div>

        <div class="gauge">
            <div class="gauge-title">Throttle</div>
            <div class="gauge-circle">
                <div class="gauge-fill throttle" id="throttleFill"></div>
                <div class="gauge-value">
                    <span id="throttleValue">0</span>
                    <div class="gauge-unit">%</div>
                </div>
            </div>
            <div class="gauge-range">0 - 100%</div>
        </div>

        <div class="gauge">
            <div class="gauge-title">Fuel Economy</div>
            <div class="gauge-circle">
                <div class="gauge-fill fuel" id="fuelFill"></div>
                <div class="gauge-value">
                    <span id="fuelValue">0.0</span>
                    <div class="gauge-unit">km/L</div>
                </div>
            </div>
            <div class="gauge-range">0 - 30 km/L</div>
        </div>

        <div class="gauge">
            <div class="gauge-title">Temperature</div>
            <div class="gauge-circle">
                <div class="gauge-fill temp" id="tempFill"></div>
                <div class="gauge-value">
                    <span id="tempValue">0</span>
                    <div class="gauge-unit">°C</div>
                </div>
            </div>
            <div class="gauge-range">0 - 120°C</div>
        </div>
    </div>

    <a href="/expert" class="expert-btn" id="expertBtn">Expert Mode</a>

    <script>
        class Dashboard {
            constructor() {
                this.ws = null;
                this.connect();
            }
            
            connect() {
                const wsUrl = `ws://${window.location.host}/ws`;
                this.ws = new WebSocket(wsUrl);
                
                this.ws.onopen = () => {
                    this.updateStatus(true);
                };
                
                this.ws.onclose = () => {
                    this.updateStatus(false);
                    setTimeout(() => this.connect(), 3000);
                };
                
                this.ws.onmessage = (event) => {
                    try {
                        const data = JSON.parse(event.data);
                        this.handleData(data);
                    } catch (e) {
                        console.error('Parse error:', e);
                    }
                };
            }
            
            updateStatus(connected) {
                const dot = document.getElementById('statusDot');
                const text = document.getElementById('statusText');
                
                if (connected) {
                    dot.classList.add('connected');
                    text.textContent = 'Connected';
                } else {
                    dot.classList.remove('connected');
                    text.textContent = 'Disconnected';
                }
            }
            
            handleData(data) {
                if (data.type === 'dashboardData') {
                    this.updateGauge('speed', data.speed, 200);
                    this.updateGauge('rpm', data.rpm, 8000);
                    this.updateGauge('throttle', data.throttle, 100);
                    this.updateGauge('fuel', data.fuel, 30);
                    this.updateGauge('temp', data.temp, 120);
                }
            }
            
            updateGauge(type, value, max) {
                const valueEl = document.getElementById(type + 'Value');
                const fillEl = document.getElementById(type + 'Fill');
                
                if (valueEl && fillEl) {
                    if (type === 'fuel') {
                        valueEl.textContent = value.toFixed(1);
                    } else {
                        valueEl.textContent = Math.round(value);
                    }
                    
                    const percentage = Math.min((value / max) * 100, 100);
                    const rotation = (percentage / 100) * 270; // 270 degrees for 3/4 circle
                    
                    if (percentage <= 25) {
                        fillEl.style.borderTopColor = 'transparent';
                        fillEl.style.borderRightColor = 'transparent';
                        fillEl.style.borderBottomColor = 'transparent';
                        fillEl.style.borderLeftColor = fillEl.classList.contains('speed') ? '#00ff88' : 
                                                      fillEl.classList.contains('rpm') ? '#ff6b35' :
                                                      fillEl.classList.contains('throttle') ? '#4ecdc4' :
                                                      fillEl.classList.contains('fuel') ? '#ffe66d' : '#ff4757';
                    } else if (percentage <= 50) {
                        fillEl.style.borderTopColor = fillEl.classList.contains('speed') ? '#00ff88' : 
                                                      fillEl.classList.contains('rpm') ? '#ff6b35' :
                                                      fillEl.classList.contains('throttle') ? '#4ecdc4' :
                                                      fillEl.classList.contains('fuel') ? '#ffe66d' : '#ff4757';
                        fillEl.style.borderRightColor = 'transparent';
                        fillEl.style.borderBottomColor = 'transparent';
                        fillEl.style.borderLeftColor = fillEl.classList.contains('speed') ? '#00ff88' : 
                                                      fillEl.classList.contains('rpm') ? '#ff6b35' :
                                                      fillEl.classList.contains('throttle') ? '#4ecdc4' :
                                                      fillEl.classList.contains('fuel') ? '#ffe66d' : '#ff4757';
                    } else if (percentage <= 75) {
                        const color = fillEl.classList.contains('speed') ? '#00ff88' : 
                                     fillEl.classList.contains('rpm') ? '#ff6b35' :
                                     fillEl.classList.contains('throttle') ? '#4ecdc4' :
                                     fillEl.classList.contains('fuel') ? '#ffe66d' : '#ff4757';
                        fillEl.style.borderTopColor = color;
                        fillEl.style.borderRightColor = color;
                        fillEl.style.borderBottomColor = 'transparent';
                        fillEl.style.borderLeftColor = color;
                    } else {
                        const color = fillEl.classList.contains('speed') ? '#00ff88' : 
                                     fillEl.classList.contains('rpm') ? '#ff6b35' :
                                     fillEl.classList.contains('throttle') ? '#4ecdc4' :
                                     fillEl.classList.contains('fuel') ? '#ffe66d' : '#ff4757';
                        fillEl.style.borderTopColor = color;
                        fillEl.style.borderRightColor = color;
                        fillEl.style.borderBottomColor = color;
                        fillEl.style.borderLeftColor = color;
                    }
                }
            }
        }
        
        // Initialize dashboard
        new Dashboard();
        
        // Demo data for testing (remove in production)
        setInterval(() => {
            const mockData = {
                type: 'dashboardData',
                speed: Math.random() * 120,
                rpm: Math.random() * 6000,
                throttle: Math.random() * 100,
                fuel: Math.random() * 25,
                temp: 80 + Math.random() * 20
            };
            // Uncomment for testing: handleData(mockData);
        }, 1000);
    </script>
</body>
</html>
)rawliteral";


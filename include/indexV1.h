// #include <Arduino.h>

// const char index_html[] PROGMEM = R"rawliteral(
// <!DOCTYPE html>
// <html>
// <head>
//   <title>CAN Live</title>
//   <script src="/chart.js"></script>
// </head>
// <body>
//   <h1>Gráficos Live do ESP32-S3</h1>
//   <canvas id="g1" width="400" height="150"></canvas>
//   <canvas id="g2" width="400" height="150"></canvas>
//   <canvas id="g3" width="400" height="150"></canvas>
//   <script>
//     const charts = {};
//     ['g1', 'g2', 'g3'].forEach(id => {
//       const ctx = document.getElementById(id).getContext('2d');
//       charts[id] = new Chart(ctx, {
//         type: 'line',
//         data: {
//           labels: [],
//           datasets: [{
//             label: id,
//             borderColor: 'blue',
//             data: [],
//             fill: false
//           }]
//         },
//         options: {
//           animation: false,
//           responsive: true,
//           scales: { y: { min: 0, max: 255 } }
//         }
//       });
//     });

//     const socket = new WebSocket("ws://" + location.host + "/ws");

//     socket.onmessage = function(event) {
//       const data = JSON.parse(event.data);
//       const now = new Date().toLocaleTimeString();

//       if (data.g1 !== undefined) {
//         charts['g1'].data.labels.push(now);
//         charts['g1'].data.datasets[0].data.push(data.g1);
//         if (charts['g1'].data.labels.length > 20) {
//           charts['g1'].data.labels.shift();
//           charts['g1'].data.datasets[0].data.shift();
//         }
//         charts['g1'].update();
//       }

//       if (data.g2 !== undefined) {
//         charts['g2'].data.labels.push(now);
//         charts['g2'].data.datasets[0].data.push(data.g2);
//         if (charts['g2'].data.labels.length > 20) {
//           charts['g2'].data.labels.shift();
//           charts['g2'].data.datasets[0].data.shift();
//         }
//         charts['g2'].update();
//       }

//       if (data.g3 !== undefined) {
//         charts['g3'].data.labels.push(now);
//         charts['g3'].data.datasets[0].data.push(data.g3);
//         if (charts['g3'].data.labels.length > 20) {
//           charts['g3'].data.labels.shift();
//           charts['g3'].data.datasets[0].data.shift();
//         }
//         charts['g3'].update();
//       }
//     };
//   </script>
// </body>
// </html>
// )rawliteral";


#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>CAN Live</title>
  <script src="/chart.js"></script>
</head>
<body>
  <h2>CAN Live Graphs</h2>
  <canvas id="g1" height=30></canvas>
  <canvas id="g2" height=30></canvas>
  <canvas id="g3" height=30></canvas>
  <canvas id="g4" height=30></canvas>

  <script>
    const ws = new WebSocket(`ws://${location.host}/ws`);
    
    const createChart = (ctx, label, color) => new Chart(ctx, {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          label: label,
          borderColor: color,
          data: [],
          fill: false,
        }]
      },
      options: {
        animation: false,
        scales: {
          x: { display: true },
          y: { 
            min : 0,
            max : 255
          }
        }
      }
    });

    const charts = [
      createChart(document.getElementById('g1').getContext('2d'), 'Byte 4 (0x280)', 'red'),
      createChart(document.getElementById('g2').getContext('2d'), 'Byte 1 (0x380)', 'blue'),
      createChart(document.getElementById('g3').getContext('2d'), 'Byte 0 (0x488)', 'green'),
      createChart(document.getElementById('g4').getContext('2d'), 'Combined Sum', 'orange')
    ];

    const maxPoints = 50;

    ws.onmessage = (msg) => {
      const data = JSON.parse(msg.data);
      const now = new Date().toLocaleTimeString();

      const values = [data.g1, data.g2, data.g3, data.g1 + data.g2 + data.g3];

      values.forEach((val, i) => {
        const chart = charts[i];
        chart.data.labels.push(now);
        chart.data.datasets[0].data.push(val);
        if (chart.data.labels.length > maxPoints) {
          chart.data.labels.shift();
          chart.data.datasets[0].data.shift();
        }
        chart.update();
      });
    };
  </script>
</body>
</html>
)rawliteral";


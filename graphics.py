import serial
import matplotlib.pyplot as plt
import json
from datetime import datetime
from collections import deque

# CONFIG
SERIAL_PORT = "COM7"       # Change to your serial port
BAUD_RATE = 115200
TARGET_ID = "0x488"
BUFFER_SIZE = 100 # How many points to keep in the live window

# Initialize buffers
timestamps = deque(maxlen=BUFFER_SIZE)
byte_values = {f"b{i}": deque(maxlen=BUFFER_SIZE) for i in range(7)}

# Set up real-time plot
plt.ion()
fig, axs = plt.subplots(7, 1, figsize=(20, 14), sharex=True)
fig.subplots_adjust(hspace=0.5)
lines = []

for i in range(7):
    axs[i].set_ylabel(f"Byte {i}")
    axs[i].set_ylim(0, 255)
    axs[i].grid(True)
    line, = axs[i].plot([], [], label=f"Byte {i}")
    axs[i].legend(loc="upper right")
    lines.append(line)

axs[-1].set_xlabel("Samples")

# Connect to serial
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud")

try:
    while True:
        try:
            line_raw = ser.readline().decode("utf-8").strip()
            # print(line_raw)
            # if not line_raw:
            #     continue

            # # Pre-clean the string and parse
            # # line_raw = line_raw.replace("'", '"')
            # data = json.loads(line_raw)
            data = line_raw
            print(data)
            try:
                v0, v1, v2, v3, v4, v5, v6, v7, v8 = data.split(" ")
            except:
                continue
            values = [v2, v3, v4, v5, v6, v7, v8]
            print(values)
            if TARGET_ID in v1:
                timestamps.append(datetime.now().strftime("%H:%M:%S"))

                if len(values) < 8:
                    continue  # skip invalid data
                for i in range(7):
                    byte_values[f"b{i}"].append(int(values[i]))
                print("troleii")
                continue
                # Update plots
                for i in range(7):
                    lines[i].set_xdata(list(range(len(byte_values[f"b{i}"]))))
                    lines[i].set_ydata([int(v) for v in byte_values[f"b{i}"]])
                    axs[i].set_xlim(0, len(byte_values[f"b{i}"]))
                    axs[i].relim()
                    axs[i].autoscale_view()
                    print("Troleiii")
                plt.pause(1)
                fig.canvas.draw()
                fig.canvas.flush_events()

        except json.JSONDecodeError:
            pass
        except Exception as e:
            print("Error:", e)

except KeyboardInterrupt:
    print("\nStopped by user")

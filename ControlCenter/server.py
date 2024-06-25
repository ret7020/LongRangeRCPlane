from flask import Flask, render_template, jsonify
import time
import serial, threading
from config import *

def readDebugUart(app):
    while 1:
        roll, pitch, yaw = map(float, app.serial.readline().decode("utf-8").strip().split(";")[0:3])
        app.debugData = {"roll": roll, "pitch": pitch, "yaw": yaw, "time": int(time.time()) - start_time}


start_time = time.time()
app = Flask(__name__)
app.serial = None

if DEBUG_FROM_UART:
    app.serial = serial.Serial("/dev/ttyUSB0", 115200)
    app.debugData = {"roll": 0, "pitch": 0, "yaw": 0}
    threading.Thread(target=lambda: readDebugUart(app)).start()

if DEBUG_FROM_MOCK:
    app.debugData = {"roll": 10, "pitch": 10, "yaw": 10}




app.config["TEMPLATES_AUTO_RELOAD"] = True

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/api/repeater/metrics")
def plane_metrics():
    return jsonify({"latest": 20, "source": 'repeater', "metrics": {"roll": 0, "pitch": 20}, 
                    "units": {"roll": "deg", "pitch": "deg"}})

if DEBUG_FROM_UART or DEBUG_FROM_MOCK:
    @app.route("/api/debug/metrics")
    def debug_metrics():
        return jsonify({"latest": 20, "source": "uart_imu", "metrics": app.debugData, 
                        "units": {"roll": "deg", "pitch": "deg", "time": "s"}})

if __name__ == "__main__":    
    app.run(host="127.0.0.1", port=8080)

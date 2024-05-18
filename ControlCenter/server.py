from flask import Flask, render_template, jsonify


app = Flask(__name__)
app.config["TEMPLATES_AUTO_RELOAD"] = True

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/api/repeater/metrics")
def plane_metrics():
    return jsonify({"latest": 20, "source": 'test', "metrics": {"roll": 20, "pitch": 20}, "units": {"roll": "deg"}})

if __name__ == "__main__":    
    app.run(host="127.0.0.1", port=8080)

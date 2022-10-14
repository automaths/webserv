from flask import Flask

app = Flask(__test__)

@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"
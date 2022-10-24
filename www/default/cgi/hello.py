from wsgiref.handlers import CGIHandler
from flask import Flask

import sys
from flask import request

app = Flask(__name__)

@app.route("/")
def hello_world():
    user = request.args.get('user')
    print(user, file=sys.stderr)
    username = request.args.get('username')
    print(username, file=sys.stderr)
    return "<p>Hello, World!</p>"

@app.route("/test")
def test():
    return "<p>This is a test!</p>"

CGIHandler().run(app)
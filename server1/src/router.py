import flask

app = flask.Flask(__name__)

@app.route('/', methods=['GET'])
def home():
    return "Router API running -_-"

app.run(host="0.0.0.0", port=80)
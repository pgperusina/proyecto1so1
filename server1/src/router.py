import flask

app = flask.Flask(__name__)

@app.route('/', methods=['GET'])
def home():
    return "Router API running -_-"

app.run()
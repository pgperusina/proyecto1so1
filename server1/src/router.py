import flask
from flask import request

app = flask.Flask(__name__)

@app.route('/', methods=['GET'])
def home():
    return "Router API running!! :)"

@app.route('/objeto', methods=['POST'])
def post_data():
    json_data = request.get_json(force=True, silent=True, cache=False)
    if request.content_length == 0:
        return "No data sent"
    elif json_data is None:
        return "Error parsing object - " + request.get_data(cache=False, as_text=True)
    else:
        print(json_data)
        return "Data is correct :)"

app.run(host="0.0.0.0", port=80)
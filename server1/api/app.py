from flask import Flask
from flask import request
from flask_api import status
from pymongo import MongoClient
import requests

SERVER_A = "server-a"
SERVER_B = "server-b"

app = Flask(__name__)

def get_documents_count(server):
    try:
        r = requests.get(server + "/getDocumentsCount")
        if r.status_code == requests.codes.ok:
            return r.text
        else:
            app.logger.info("Error getting documents count from '%s'. Status code: '%s'", server, str(r.status_code))
            return None
    except Exception as e:
        app.logger.info('Error getting documents count from %s. -- %s', server, e)
        return None

def get_ram(server):
    try:
        r = requests.get(server + "/getRamUsage")
        if r.status_code == requests.codes.ok:
            return r.text
        else:
            app.logger.info("Error getting RAM usage from '%s'. Status code: '%s'", server, str(r.status_code))
            return None
    except Exception as e:
        app.logger.info('Error getting RAM usage from %s. -- %s', server, e)
        return None

def post_to_server(server, json_data):
    try:
        r = requests.post(server + "/postDocument", data = json_data)
        if r.status_code == requests.codes.ok:
            return r.text
        else:
            app.logger.info("Error posting document to '%s'. Status code: '%s'", server, str(r.status_code))
            return None
    except Exception as e:
        app.logger.info("Error posting document to '%s'. -- %s", server, e)
        return None

def post_based_on_counting(a_count, b_count, json_data):
    if isinstance(a_count, None) or isinstance(b_count, None):
        app.logger.info("Error, document count not available")
        return None
    elif a_count > b_count:
        result = post_to_server(SERVER_B, json_data)
        if isinstance(result, None):
            app.logger.info("Error posting document to '%s'.", SERVER_B)
            return None
        else:
            app.logger.info("Document '%s' posted to '%s'.", json_data, SERVER_B)
            return result
    elif a_count < b_count:
        result = post_to_server(SERVER_A, json_data)
        if isinstance(result, None):
            app.logger.info("Error posting document to '%s'.", SERVER_A)
            return None
        else:
            app.logger.info("Document '%s' posted to '%s'.", json_data, SERVER_A)
            return result
    elif a_count == b_count:
        return None

def post_based_on_ram(a_ram, b_ram, json_data):
    if isinstance(a_ram, None) or isinstance(b_ram, None):
        app.logger.info("Error, RAM usage not available")
        return None
    elif a_ram > b_ram:
        result = post_to_server(SERVER_B, json_data)
        if isinstance(result, None):
            app.logger.info("Error posting document to '%s'.", SERVER_B)
            return None
        else:
            app.logger.info("Document '%s' posted to '%s'.", json_data, SERVER_B)
            return result
    elif a_ram < b_ram:
        result = post_to_server(SERVER_A, json_data)
        if isinstance(result, None):
            app.logger.info("Error posting document to '%s'.", SERVER_A)
            return None
        else:
            app.logger.info("Document '%s' posted to '%s'.", json_data, SERVER_A)
            return result
    elif a_ram == b_ram:
        return None

@app.route('/', methods=['GET'])
def home():
    return "LoadBalancer API running!! :)", status.HTTP_200_OK

@app.route('/document', methods=['POST'])
def post_data():
    json_data = request.get_json(force=True, silent=True, cache=False)
    if request.content_length == 0:
        app.logger.info("No data sent to post")
        return str("No data sent"), status.HTTP_500_INTERNAL_SERVER_ERROR
    elif json_data is None:
        app.logger.info("Error parsing object to post")
        return str("Error parsing object - " + request.get_data(cache=False, as_text=True)), status.HTTP_500_INTERNAL_SERVER_ERROR
    else:
        a_count = int(get_documents_count(SERVER_A))
        b_count = int(get_documents_count(SERVER_B))
        
        ##### Posting to server based on mongoDB documents counting #####
        response = post_based_on_counting(a_count, b_count, json_data)
        if not isinstance(response, None):
            return str(response), status.HTTP_200_OK

        #### Posting to server based on server RAM usage
        else:
            a_ram = int(get_ram(SERVER_A))
            b_ram = int(get_ram(SERVER_B))

            response = post_based_on_ram(a_ram, b_ram, json_data)
            if not isinstance(response, None):
                return str(response), status.HTTP_200_OK
        
            #### Posting to server based on server CPU usage
            else:
                a_cpu = int(get_cpu(SERVER_A))
                b_cpu = int(get_cpu(SERVER_B))

                response = post_based_on_cpu(a_cpu, b_cpu, json_data)
                if not isinstance(response, None):
                    return str(response), status.HTTP_200_OK

                #### Posting to server A
                else:
                    result = post_to_server(SERVER_A, json_data)
                    if isinstance(result, None):
                        app.logger.info("Error posting document to '%s'.", SERVER_A)
                        return str("Error posting document to server " + SERVER_A), status.HTTP_500_INTERNAL_SERVER_ERROR
                    else:
                        app.logger.info("Document '%s' posted to '%s'.", json_data, SERVER_A)
                        return str(result)

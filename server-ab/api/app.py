from flask import Flask
from flask_api import status
from flask import request
from pymongo import MongoClient

app = Flask(__name__)
try:
    client = MongoClient("mongodb://root:rootpass@mongodb:27017/sopes1?authSource=admin")
    db = client.sopes1
except Exception as e:
    app.logger.info('Error connection to mongoDB - %s', e)

def get_db_name():
    return db.name

@app.route('/dbname', methods=['GET'])
def db_name():
    try:
        return str('Hello! DB name is {}'.format(get_db_name())), status.HTTP_200_OK
    except Exception as e:
        app.logger.info('Error in DB - %s', e)
        return str("Error in DB"), status.HTTP_500_INTERNAL_SERVER_ERROR

@app.route("/getDocumentsCount", methods=['GET'])
def get_count():
    try:
        count = db.proyecto1.count_documents({})
        print("documents count... " + str(count))
        return str(count), status.HTTP_200_OK
    except Exception as e:
        print("error getting documents count")
        print(e)
        app.logger.info('Error getting documents count - %s', e)
        return str("Error getting documents count."), status.HTTP_500_INTERNAL_SERVER_ERROR

@app.route("/postDocument", methods=['POST'])
def post_document():
    try:
        json_data = request.get_json(force=True, silent=True, cache=False)
        if request.content_length == 0:
            print("no data sent")
            app.logger.info('No data sent')
            return str("No data sent") , status.HTTP_500_INTERNAL_SERVER_ERROR
        elif json_data is None:
            print("error parsing object...")
            app.logger.info('Error parsing object')
            return str("Error parsing object - " + request.get_data(cache=False, as_text=True)) , status.HTTP_500_INTERNAL_SERVER_ERROR
        else:
            post_id = db.proyecto1.insert_one(json_data).inserted_id
            app.logger.info('Successful post - %s', str(post_id))
            return str(post_id), status.HTTP_200_OK
    except Exception as e:
        print("Error posting document to mongo DB")
        print(e)
        app.logger.info('Error posting documents to mongoDB')
        return str("Error posting documents to mongoDB."), status.HTTP_500_INTERNAL_SERVER_ERROR 

@app.route("/getRamUsage", methods=['GET'])
def get_vm_ram_usage():
    try:
        file = open("/host/proc/meminfo")
        line = file.read()
        file.close()
        return str(line)
    except Exception as e:
        print("Error VM RAM Usage print ---- ", + str(e))
        app.logger.info('Error getting VM RAM usage - %s', e)
        return str("Error getting VM RAM usage"), status.HTTP_500_INTERNAL_SERVER_ERROR
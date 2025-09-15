import os
import sqlite3
import json
import io
import configparser
from flask import Flask, render_template, jsonify, request, send_file, abort

class FlaskApp:
    def __init__(self):
        self.app = Flask(__name__)
        self.setup_routes()
        
        self.config = configparser.ConfigParser()
        self.config.read('Configure.ini')

        
    def setup_routes(self):
        app = self.app

        @app.route('/')
        def hello():
            return render_template('index.html')

        @app.route('/test')
        def hi():
            return render_template('test.html')

        @app.route('/camera')
        def camera():
            return render_template('camera.html')

        @app.route('/album/<albumid>')
        def album(albumid):
            return render_template('album.html', albumId=albumid)

        @app.route('/trip/<tripid>')
        def trip_page(tripid):
            return render_template('map.html', tripId=tripid)

        @app.route('/api/images/<city>')
        def get_city_images(city):
            base_path = f"static/logs/cities/{city}"
            base_path_images = f"{base_path}/images"
            return_path = f"../{base_path_images}"

            if not os.path.exists(base_path_images) or not os.listdir(base_path_images):
                return jsonify({"error": "City not found"}), 404

            files = os.listdir(base_path_images)
            images = {}
            valid_file_types = ('.webp', '.png', '.jpeg', '.gif')
            for i, file in enumerate(files):
                if file.endswith(valid_file_types):
                    image_id = f"{city}_{i}"
                    properties = {
                        "lat": 21.0285,
                        "lng": 105.8542,
                        "filepath": f"{return_path}/{file}",
                        "caption": f"dsdsdsd_{i}",
                        "location": f"test_{i}",
                        "time": f"timeTest_{i}"
                    }
                    images[file] = properties  # fixed structure

            info_json_path = f'{base_path}/{city}.json'
            with open(info_json_path, 'w', encoding='utf-8') as w:
                json.dump(images, w, ensure_ascii=False, indent=2)

            return jsonify(images)

        # @app.route('/api/imagedelete', methods=['POST'])
        # def delete_image():
        #     data = request.get_json()
        #     city = data.get('city')
        #     filename = data.get('filename')

        #     base_path = f'static/logs/cities/{city}'
        #     image_path = f'{base_path}/images/{filename}'
        #     info_path = f'{base_path}/{city}.json'

        #     if os.path.exists(image_path) and os.path.exists(info_path):
        #         os.remove(image_path)
        #         with open(info_path, 'r', encoding='utf-8') as r:
        #             info_data = json.load(r)

        #         new_info_data = [item for item in info_data if item.get("filename") != filename]

        #         with open(info_path, 'w', encoding='utf-8') as w:
        #             json.dump(new_info_data, w, ensure_ascii=False, indent=2)

        #         return jsonify({"message": "Success!"})
        #     else:
        #         return jsonify({"message": "Image path doesn't exist!"})

        @app.route("/api/gettrips")
        def get_trips():
            trip_path = self.config['Database']['tripDetail']
            triplist = []

            with open(trip_path, 'r') as r:
                data = json.load(r)
                triplist = data.get('trip', [])

            return jsonify(triplist)

        @app.route("/api/getcurenttripid")
        def get_current_trip_id():
            tripId = self.config['Settings']['tripID']
            return jsonify(tripId)

        @app.route("/api/getdatas/<tripid>")
        def get_datas(tripid):
            db_path = self.config['Database']['gpsPath']
            con = sqlite3.connect( db_path)
            cur = con.cursor()
            cur.execute(f'''SELECT * FROM log WHERE tripId = ? ORDER BY time ASC''',(tripid,))
            columns = [col[0] for col in cur.description]  # get column names
            rows = cur.fetchall()
            result = [dict(zip(columns, row)) for row in rows]
            con.close() 
            return jsonify(result)
        @app.route("/api/delete", methods =['POST'])
        def delete_image():
            data = request.get_json()
            imageid = data.get('imageid')
            fullImageid = f'static/logs/gallery/{imageid}'
            db_path = self.config['Database']['gpsPath']
            print(imageid)
            print(fullImageid)
            print(db_path)
            con = sqlite3.connect(db_path)
            cur = con.cursor()
            cur.execute('UPDATE log SET path = ? WHERE path = ?',(None,imageid))
            con.commit()
            con.close()
            if(os.path.exists(fullImageid) and os.path.exists(db_path)):
                os.remove(fullImageid)
                return jsonify({"message":"Delete!"})
            
            else:
                return jsonify({"message":"Path doesnt exist !"})
            
        
        # @app.route("/api/getim` ages/<tripid>")
        # def  get_image(tripid):
        #     db_path = self.config['Database']['albumsDB']
        #     con = sqlite3.connect(db_path)
        #     cur = con.cursor()
        #     cur.execute(f'SELECT * FROM album WHERE id = ? ORDER BY time ASC',(tripid))
        #     columns = [col[0] for col in cur.description]  # get column names
        #     rows = cur.fetchall()

        #     result = [dict(zip(columns, row)) for row in rows]
        #     con.close() 

        #     db_paths = self.config['Database']['gpsPath']
        #     con = sqlite3.connect(db_paths)
        #     cur = con.cursor()
        #     for re in result:
        #         cur.execute('''
        #         INSERT INTO log (time, path, type)
        #         VALUES (?, ?, ?)
        #         ON CONFLICT(time) DO UPDATE SET
        #             path = excluded.path,
        #             type = excluded.type
        #         ''', (re['time'], re['path'], re['type']))    
        #         con.commit()        
            
        #     con.close()
        #     return jsonify(result)
           
    def run(self):
        self.app.run(host='0.0.0.0', port=5000)

if __name__ == '__main__':
    server = FlaskApp()
    server.app
    
server = FlaskApp()
app = server.app 

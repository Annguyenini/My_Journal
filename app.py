import os
from flask import Flask ,render_template, jsonify,json,request,send_file, abort, g
import sqlite3
import io
# from flask_sslify import SSLify
app = Flask(__name__)
# sslify = SSLify(app)

    


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
    return render_template('album.html',albumId=albumid)



@app.route('/api/images/<city>')
def get_city_images(city):
    # base_path_images = f"static/asset/images/{city}"
    base_path = f"static/logs/cities/{city}"
    base_path_images = f"static/logs/cities/{city}/images"
    return_path = f"../"+base_path_images
    if not os.listdir(base_path_images):
        return jsonify({"error":"City not found"}),404
    files = os.listdir(base_path_images)
    images={}
    valid_file_type = ('.webp','.png','.jpeg','.gif')
    for i, file in enumerate (files):
        if file.endswith(valid_file_type):
            # images.append(f"{return_path}/{file}" )
            image_id = f"{city}_{i}"
            properties =  {
                "lat":21.0285,
                "lng":105.8542,
                "filepath": f"{return_path}/{file}",
                "caption":f"dsdsdsd_{i}",
                "location":f"test_{i}",
                "time":f"timeTest_{i}"
            }
            images[file][properties].append(properties)
    info_json_path = f'{base_path}/{city}.json'
    

    with open(info_json_path,'w',encoding='utf-8') as w:
        
        json.dump(images, w, ensure_ascii=False, indent=2)
     
    print(images)
    return jsonify(images)

@app.route('/api/imagedelete', methods=['POST'])
def delete_image():
    data = request.get_json()
    print(data)
    city=data.get('city')
    base_path = f'static/logs/cities/{city}'
    filename=data.get('filename')
    image_path = f'{base_path}/images/{filename}'
    info_path = f'{base_path}/{city}.json'

    new_info_data =[]
    print(image_path)
    if os.path.exists(image_path) and os.path.exists(info_path):
        os.remove(image_path)
        with open (info_path,'r',encoding='utf-8')as r:
            info_data = json.load(r)  
        for item in (info_data):
            if item["filename"] != filename:
                # item["id"]=f"{city}_{i}"
                new_info_data.append(item)
        with open(info_path,'w',encoding ='utf-8') as w:
            json.dump(new_info_data, w, ensure_ascii=False, indent=2)
        return jsonify({"message":"Success!"})
    else:
        return jsonify({"message":"image path doesnt exist!"})



# def location_update(lat,lng, event= "update every 10s"):


if __name__ =='__main__':
    # app.run(host='0.0.0.0',port =5000,ssl_context='adhoc')
    app.run(host='0.0.0.0',port =5000)


    
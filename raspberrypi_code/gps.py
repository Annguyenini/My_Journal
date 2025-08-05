import serial
import pynmea2
import time
import json
import os
import sqlite3 
from datetime import datetime
from shapely.geometry import shape, Point
import threading 
class GPS:
    def __init__(self):
        self.port="/dev/serial0"
        self.ser = serial.Serial(self.port, baudrate=9600, timeout=1) 
        self.con = None
        self.cur = None
        self.table_name = None
        self.DB_path = None
        self.current_day= datetime.now().strftime('%Y_%m_%d') # get time
        self.current_city= None
        self.current_city_real_name =None
        self.speed_kmh = 0
        # self.lat=23.4
        # self.lng=102.0
        self.lat=0
        self.lng=0
        self._init_path()
        self._init_db_check()
        self.gps_buffer = []
        self.lock = threading.Lock()
    def _init_path(self):
        self.script_dir = os.path.dirname(os.path.abspath(__file__))
        self.static_parent_dir = os.path.abspath(os.path.join(self.script_dir, '..','static', 'asset'))
        self.current_day = datetime.now().strftime('%Y_%m_%d') # get time
        self.DB_path = f"logs/databases/{self.current_day}.db"

    def _init_db_check(self):
        # self.DB_path = os.path.join(self.static_parent_dir,'static')
        dir_path = os.path.dirname(self.DB_path)
        if not os.path.exists(dir_path):
            os.makedirs(dir_path)
        print(dir_path)
        print(self.DB_path)
        try:
            self.con = sqlite3.connect(self.DB_path,check_same_thread=False,isolation_level=None)

            self.con.execute('PRAGMA journal_mode=WAL;') #WAL mode
            self.cur = self.con.cursor()
            self.cur.execute(f'''CREATE TABLE IF NOT EXISTS log_{self.current_day} (city TEXT, lat REAL, lng REAL, time TEXT )''')
            self.con.commit()
            print( "Database connected and table ready.")
            return True
        except Exception as e:
            print("Error initializing database:", e)
            return False

    def check_for_citiy(self,point):
        vn_path_polygon = os.path.join(self.static_parent_dir,'mapjs','vn.json')
        with open ("vn.json") as vn:
            vn_polygon = json.load(vn)
        for features in vn_polygon["features"]:
            polygon = shape(features["geometry"])
            if polygon.contains(point) or polygon.intersects(point):
                self.current_city_real_name = features['properties'].get('real_name',"Unkown city")
                return features['properties'].get('name' , "Unkown city")
        return None

    def start_gps_read(self):
        while True:
            try:
                newdata = self.ser.readline().decode('ascii', errors='replace') ## read from GPS serial port
                if newdata.startswith('$GPRMC'): ##Check if line is a GPRMC NMEA sentence
                    msg = pynmea2.parse(newdata) ##Parse the NMEA sentence
                    self.lat = msg.latitude
                    self.lng = msg.longitude

                    ####testing pupose
                    # self.lat  +=0.00001
                    # self.lng +=0.00001
                    if  msg.spd_over_grnd is not None:
                        speed_knots = float(msg.spd_over_grnd)
                        self.speed_kmh = speed_knots * 1.852   
                    else: 
                        self.speed_kmh = 0        
                    if self.current_city is None:
                        point = Point(self.lng,self.lat) # lng then lat acording to x,y
                        self.current_city=self.check_for_citiy(point)
                    current_time = datetime.now().strftime('%Y_%m_%d_%H%M%S')
                    print(f"Lat={self.lat}, Lng={self.lng}, gps_time = {current_time},speed ={self.speed_kmh}")
                    self.gps_buffer.append((self.current_city,self.lat,self.lng,current_time))
                    if len(self.gps_buffer)>=10:
                        with self.lock:
                            self.cur.executemany(f'''INSERT INTO log_{self.current_day} (city,lat, lng, time) VALUES(?,?,?,?)''',[(self.current_city,self.lat,self.lng,current_time)])
                            self.con.commit()
                time.sleep(0.05)
            except pynmea2.ParseError:
                continue
            except KeyboardInterrupt:
                if self.gps_buffer:
                    with self.lock:
                        self.cur.executemany(f'''INSERT INTO log_{self.current_day} (city,lat, lng, time) VALUES(?,?,?,?)''',[(self.current_city,self.lat,self.lng,current_time)])
                        self.con.commit()
                print("Stopping GPS read.")
                break
    def speed_monitor(self):
        while True:

            with self.lock:
                spd = self.speed_kmh
                lat =self.lat
                lng =self.lng
            point = Point(lng,lat)  # lng then lat acording to x,y

            if spd>= 40 :
                self.current_city=self.check_for_citiy(point)
                time.sleep(30)
                
            elif spd>= 20 and spd<40:
                self.current_city=self.check_for_citiy(point)
                time.sleep(40)
                
            elif spd <20 :
                self.current_city=self.check_for_citiy(point)
                time.sleep(60)

    def get_data_fromDB(self,input_time):
        self.cur.execute(f'''SELECT * FROM log_{self.current_day} WHERE time = ?''',(input_time,))
        row = self.cur.fetchone()
        return row

    def get_coordinate(self):
        return (self.lat,self.lng)
    
    def get_current_city(self):
        return self.current_city_real_name
    def start_thread(self):
        threading.Thread(target=self.start_gps_read, daemon=True).start()
        threading.Thread(target=self.speed_monitor, daemon=True).start()
        print("Runing gps")
        while True:
            time.sleep(1)


gps =GPS()
gps.start_thread()

import sqlite3
import os
import time
from datetime import datetime
class DB:
    def __init__(self):
        self.parent_dir = os.path.dirname(os.path.abspath(__file__))
        self.album_db_path=f"{self.parent_dir}/cities/album.db"
        self.current_day= datetime.now().strftime('%Y_%m_%d') # get time
        self.set_up_db()
    def set_up_db(self):
        try:
            self.con = sqlite3.connect(self.album_db_path,check_same_thread=False,isolation_level=None)
            print(f"[test[ {self.album_db_path}")
            self.con.execute('PRAGMA journal_mode=WAL;') #WAL mode

            self.cur = self.con.cursor()
            self.cur.execute(f'''CREATE TABLE IF NOT EXISTS album (path TEXT, type TEXT CHECK(type IN ('image', 'video')), city TEXT, time TEXT, lat TEXT, lng TEXT, cap TEXT) ''')
            #path #type #city #time #lat #lng # cap
            self.con.commit()
            print("DATABASE OK")
        except Exception as e:
            print(f"[ERROR!! WITH DATABASE]: {e}")
    def add_to_db(self, entry):
        try:
            path = entry['path']
            dtype = entry['type']
            city = entry['city']
            time = entry['time']
            lat = entry['lat']
            lng=entry['lng']
            cap = entry['cap']
            self.cur.execute(f'''INSERT INTO album (path,type,city,time,lat,lng,cap) VALUES(?,?,?,?,?,?,?)''',(path,dtype,city, time, lat, lng,cap))
            self.con.commit()
            return
        except Exception as e:
            print(f"[ERORR With Inserting]:{e}")
            return
    def get_all_from_album(self):
        try:
            self.cur.execute('''SELECT path FROM album ORDER BY time DESC''')
            row = [row[0]for row in self.cur.fetchall()]
            return row
        except Exception as e:
            print(e)
            return []

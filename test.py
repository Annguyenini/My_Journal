import sqlite3
import os
import zlib

mbtiles_path = 'static/dsdsd.mbtiles'
output_dir = 'tiles'

conn = sqlite3.connect(mbtiles_path)
cursor = conn.cursor()
cursor.execute("SELECT zoom_level, tile_column, tile_row, tile_data FROM tiles")

for z, x, y, tile_data in cursor.fetchall():
    y = (1 << z) - 1 - y  # Flip Y for XYZ format
    dir_path = os.path.join(output_dir, str(z), str(x))
    os.makedirs(dir_path, exist_ok=True)
    with open(os.path.join(dir_path, f'{y}.png'), 'wb') as f:
        f.write(tile_data)

conn.close()
from flask import Flask, render_template_string
import folium
import webview
app = Flask(__name__)

@app.route('/')
def map_view():
    # Center coordinates
    lat, lng = 10.141887723981544, 105.32414513618923

    # Create map with local tiles
    

    # Embed webview
    webview_frame = tk.Frame(window)
    webview_frame.pack(fill="both", expand=True)

    # Create webview window inside Tkinter
    webview.create_window("Leaflet Map", "http://127.0.0.1:5000", gui="tk")
    webview.start(gui="tk", debug=False)

if __name__ == '__main__':
    app.run(debug=True)

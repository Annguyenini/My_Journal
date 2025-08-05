from flask import Flask, Response
from picamera2 import Picamera2
from picamera2.previews.qt import QPicamera2
from picamera2.encoders import H264Encoder
from PIL import Image
import io
import time
from PyQt5.QtCore import QUrl,QTimer, QTime,Qt,QThread,pyqtSignal
from PyQt5.QtGui import QImage, QPixmap,QTransform
from PyQt5.QtWidgets import (
    QApplication,
    QCheckBox,
    QComboBox,
    QDateEdit,
    QDateTimeEdit,
    QDial,
    QDoubleSpinBox,
    QFontComboBox,
    QLabel,
    QLCDNumber,
    QLineEdit,
    QMainWindow,
    QProgressBar,
    QPushButton,
    QRadioButton,
    QSlider,
    QSpinBox,
    QTimeEdit,
    QVBoxLayout,
    QWidget,
    QHBoxLayout,
    QSpacerItem,
    QSizePolicy,
    QMenu,
    QAction,
)

import subprocess
from gpst import GPS
from datetime import datetime
import os
from album_db import DB
def force_reset_camera():
    try:
        subprocess.run(["rpicam-still", "--timeout", "1", "-n"], timeout=3)
        print("[INFO] Dummy camera run to reset state.")
    except Exception as e:
        print(f"[WARN] Dummy reset failed: {e}")

class CAMERA_WINDOW(QWidget):
    def __init__(self, parent):
        super().__init__()
        self.parent = parent
        self.gps = GPS()     
        self.db = DB() 
        self.camera = None
        self.camera_label = None
        self.parent_dir = os.path.dirname(os.path.abspath(__file__))
        self._init_path_setup(self.gps.get_current_city())

    def _init_path_setup(self,city):
        dir = f"{self.parent_dir}/cities/{city}"
        try:
            if not os.path.exists(dir):
                os.makedirs(dir)
        except Exception as e:
            print(f"[ERROR] with path: {e}")
        return
    def start_camera(self):
        # Stop and clean any existing camera
        self.stop_camera()
        if self.camera is None:
            try:
                self.camera = Picamera2()
                config = self.camera.create_video_configuration(main={"size": (1920,1080 )}, lores={"size":(1280,720)}, display ="lores")
                self.camera.configure(config)
            except Exception as e:
                print(f"[ERROR] Failed to configure camera: {e}")
                force_reset_camera()
                self.stop_camera()
                return
            self.rotate_angle = 270
            self.transform = QTransform().rotate(self.rotate_angle)

            self.camera_label = QLabel()
            self.camera_label.setObjectName("camera_label")
            self.camera_label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

            self.parent.layout.insertWidget(3,self.camera_label)
            try:
                self.camera.start()
            except Exception as e:
                print(f"[ERROR] Failed to start camera: {e}")
            self.timer = QTimer()
            self.timer.timeout.connect(self.update_frame)
            self.timer.start(50)
        else:
            print("Camera is none")
            return
    def get_capture_path(self):
        city = self.gps.get_current_city()
        current_time = datetime.now().strftime('%Y_%m_%d_%H%M%S')
        capture_path=f"cities/{city}/{city}_{current_time}"
        return capture_path
    def snap_picture(self):
        picture_path=(f"{self.get_capture_path()}.jpg")
        self.camera.capture_file(picture_path)
        current_time = datetime.now().strftime('%Y_%m_%d_%H%M%S')
        lat,lng = self.gps.get_coordinate()
        entry ={
            'path':f'{picture_path}',
            'type':'image',
            'city':f'{self.gps.get_current_city()}',
            'time':f'{current_time}',
            'lat':lat,
            'lng':lng,
            'cap':None
        }
        print (entry)
        self.db.add_to_db(entry)
        time.sleep(0.5)
        return picture_path

    def recorder(self):
        video_path=(f"{self.get_capture_path()}.h264")
        encoder = H264Encoder(bitrate=10000000)
        self.camera.start_recording(encoder, video_path)
        time.sleep(8)
        self.camera.stop_recording()
        return video_path

    def update_frame(self):
        frame = self.camera.capture_array() # grab frame
        height, width, channels = frame.shape 
        bytes_per_line = channels * width 
        qimage = QImage(frame.data, width, height, bytes_per_line, QImage.Format_RGBA8888) # 4 channels
        pixmap = QPixmap.fromImage(qimage)  # Make pixmap from qimage

        # Rotate with QTransform
        rotated_pixmap = pixmap.transformed(self.transform, mode=Qt.SmoothTransformation)
        scaled_pixmap = rotated_pixmap.scaled(600,1000,Qt.KeepAspectRatio,Qt.SmoothTransformation)
        # Set pixmap on label
        self.camera_label.setPixmap(scaled_pixmap)
    def stop_camera(self):
        if self.camera:
            try:
                self.timer.stop()
                self.camera.stop()

                self.camera.close()
                print("stopping......")
                time.sleep(0.1)  # Let system clean up
            except Exception as e:
                print(f"[WARN] stop_camera failed: {e}")
            self.camera = None

        if self.camera_label:
            # self.parent.layout.removeWidget(self.camera_label)
            self.camera_label.deleteLater()
            self.camera_label = None

    def closeEvent(self, event):
        self.stop_camera()
        event.accept()

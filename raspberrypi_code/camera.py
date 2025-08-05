from flask import Flask, Response
from picamera2 import Picamera2, Preview
from PIL import Image
import io
import time
import cv2
from PyQt5.QtCore import QUrl,QTimer, QTime,Qt,QThread,
from PyQt5.QtGui import QImage, QPixmap
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
class PICAMERA:
    def __init__(self,parent):
        self.parent = parent
        self.camera_label = QLabel("test")
        self.camera_label.setObjectName("camera_label")

    def camera_set_up(self):
        self.camera = Picamera2()
        self.config = self.camera.configure(self.camera.create_video_configuration(main={"size": (800, 480)}))
        self.camera.start()
        self.camera_label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_frame)
        self.timer.start(50)
        self.camera_label.move(100, 100)
        self.camera_label.show()
        self.parent.layout.addWidget(self.camera_label)
        self.parent.layout.insertWidget(3,self.camera_label)
        self.update_frame()

    
    def update_frame(self):
        frame = self.camera.capture_array() # get frame from cam
        # frame_bgr = cv2.cvtColor(frame, cv2.COLOR_RGBA2BGR) #convert to bgr to mess with the filter...

        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)# convert back to RGB =))

        h, w, ch = frame_rgb.shape # frame size 
        bytes_per_line = ch * w  
        qt_img = QImage(frame_rgb.data, w, h, bytes_per_line, QImage.Format_RGB888)

        self.camera_label.setPixmap(QPixmap.fromImage(qt_img))
    
    def take_picture(self):
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        file_path = f"photo_{timestamp}.png"
        image = self.camera.capture_array()  # grab NumPy array from Picamera2
        img = Image.fromarray(image)
        img.save(file_path)

    def camera_stop(self):
        if hasattr(self, 'timer'):
            self.timer.stop()
        if hasattr(self, 'cap'):
            self.cap.release()




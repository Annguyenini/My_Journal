from flask import Flask, Response
from picamera2 import Picamera2
from picamera2.previews.qt import QPicamera2
from PIL import Image
import io
import time
from PyQt5.QtCore import QUrl,QTimer, QTime,Qt,QThread,pyqtSignal
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

import subprocess

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
        self.camera = None
        self.camera_label = None

    def start_camera(self):
        # Stop and clean any existing camera
        self.stop_camera()
        if self.camera is None:
            try:
                self.camera = Picamera2()
                config = self.camera.create_video_configuration(main={"size": (1280, 720)})
                self.camera.configure(config)
            except Exception as e:
                print(f"[ERROR] Failed to configure camera: {e}")
                force_reset_camera()
                self.stop_camera()
                return

            self.camera_label = QPicamera2(self.camera, width=480, height=270, keep_ar=False)
            self.camera_label.setObjectName("camera_label")
            self.camera_label.setFixedSize(480, 270	)
            self.parent.layout.insertWidget(3,self.camera_label)

            try:
                self.camera.start()
            except Exception as e:
                print(f"[ERROR] Failed to start camera: {e}")
        else:
            print("Camera is none")
            return

    def stop_camera(self):
        if self.camera:
            try:
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

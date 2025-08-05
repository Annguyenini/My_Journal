from PyQt5.QtCore import QUrl,QTimer, QTime,Qt,pyqtSignal
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
    QScrollArea,
    QDialog,
    QGridLayout
)
from PyQt5.QtWebEngineWidgets import QWebEngineView
from PyQt5.QtGui import QPixmap
import sys
from datetime import datetime
from gpst import GPS
from cameratest import CAMERA_WINDOW
from album_db import DB
from functools import partial

import time
class APP:
    def __init__(self):
        self._init_setup_()
        self._init_properties()
        self.active_feature = None
        self.set_menu()
        self.set_satus_bar()
        self.update_time()
        self.gps =GPS()
        # self.camera =CAMERA_WINDOW(self)
        self.set_city_label()
        self.update_current_city()
        self.gps.start_thread()
        self.set_coordinate()
        self.update_coordinate()
        self.timer()
        self.get_map()
        # self.set_buttons()
    
    def _init_setup_(self):
        self.app = QApplication(sys.argv) 
        self.window = QMainWindow() 
        self.central_widget = QWidget()
        self.central_widget.setStyleSheet("background-color: #292828;")  # dark blue-gray
        self.layout = QVBoxLayout()
        self.window.setCentralWidget(self.central_widget) 
        self.layout.setContentsMargins(0, 0, 0, 0)  # no padding around
        self.layout.setSpacing(0)  # no space between widgets
        self.central_widget.setLayout(self.layout)
        self.window.resize(480, 800)  
        self.window.showFullScreen()      

    def _init_properties(self):
        self.btn_properties = """QPushButton {
                background-color: #000000;
                color: white;
                font-size: 20px;
                padding: 10px 20px;
                border: none;
                border-radius: 8px;
            }
            QPushButton:hover {
                background-color: #005f99;
            }"""
        self.status_bar_properties =QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)


    def update_time(self):
        self.current_time = QTime.currentTime().toString("hh:mm:ss AP")
        self.clock_label.setText(f"{self.current_time}")
        # time.sleep(1)
    
    def update_current_city(self):
        current_city = self.gps.get_current_city()
        self.city_label.setText(current_city)

    def set_menu(self):
        self.option_menu = QMenu()
        exit_action = QAction("Exit",self.window)
        exit_action.triggered.connect(self.window.close)
        self.option_menu.addAction(exit_action)
        
    def set_satus_bar(self):
        self.status_bar = QHBoxLayout()
        self.clock_label = QLabel()
        self.clock_label.setObjectName ="Menu"
        self.clock_label.setStyleSheet("color: white; font-size: 15px; margin: 0; padding: 0;")  # remove internal margins
        # self.clock_label.setFixedHeight(20)  # shrink height if needed

        self.option = QPushButton("≡")
        self.option.setStyleSheet(self.btn_properties)
        self.option.setMenu(self.option_menu)

        self.battery_percentage = QLabel("battery")
        self.battery_percentage.setObjectName ="Battery"
        self.battery_percentage.setStyleSheet("color: white; font-size: 15px;")

        self.status_bar.addWidget(self.option)
        self.status_bar.addStretch(1)  # spacer
        self.status_bar.addWidget(self.clock_label)
        self.status_bar.addStretch(1)  # spacer
        self.status_bar.addWidget(self.battery_percentage)
        self.layout.addLayout(self.status_bar)
    def set_city_label(self):
        self.city_label = QLabel()
        self.city_label.setObjectName ="City"
        self.city_label.setStyleSheet("color: white; font-size: 30px; margin: 0; padding: 0;")
        self.layout.addWidget(self.city_label,alignment=Qt.AlignCenter | Qt.AlignTop)        
    
    def update_coordinate(self):
        coords = self.gps.get_coordinate()
        lat,lng = coords
        self.coordinate_label.setText(f"Lat: {lat:.6f}, Lng: {lng:.6f}")

    def set_coordinate(self):
        self.coordinate_label = QLabel()
        self.coordinate_label.setStyleSheet("color: white ;font-size: 20px; margin: 0; padding: 0;")
        self.layout.addWidget(self.coordinate_label, alignment=Qt.AlignCenter | Qt.AlignTop)    


    def get_map(self):
        self.map_label = QWebEngineView()  
        self.map_label.setObjectName ="Map"      
        # self.map_label.setGeometry(20, 30)
        self.map_label.load(QUrl("http://192.168.0.49:5000"))         
        self.window.setWindowTitle("Map Viewer") 
        self.window.show()          
        self.layout.addWidget(self.map_label)

    def run(self):
        sys.exit(self.app.exec_())
    def timer(self):
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_time)
        self.timer.timeout.connect(self.update_coordinate)
        self.timer.timeout.connect(self.update_current_city)
        self.timer.start(1000)
    def widget_checker(self):
        for i in range(self.layout.count()):
            item = self.layout.itemAt(i)
            widget = item.widget()
            if widget:
                print(f"Index {i}: {widget} - ObjectName: {widget.objectName()}")
class BUTTONS ():
    def __init__(self,parent):
        self.parent=parent
        self.camera = CAMERA_WINDOW(self.parent)
        self._init_set_up()
    

    def _init_set_up(self):
        self.button_layout = QHBoxLayout()
        self.album_btn = QPushButton("Album")
        self.log_btn = QPushButton("Logs")
        self.camera_btn = QPushButton("Camera")
        self.reload_btn =QPushButton("Reload-Map")
        # button_layout.addWidget(album_btn)
        # button_layout.addWidget(log_btn)
        # button_layout.addWidget(camera_btn)
        # button_layout.addWidget(reboot_btn)
        for btn in [self.album_btn,self.log_btn,self.camera_btn,self.reload_btn]:
            btn.setStyleSheet( self.parent.btn_properties)
            self.button_layout.addWidget(btn)
        self.album_btn.clicked.connect(self.galerry_btn_clicked)
        self.log_btn.clicked.connect(self.log_clicked)
        self.camera_btn.clicked.connect(self.camera_clicked)
        self.reload_btn.clicked.connect(self.parent.map_label.reload)
        self.parent.layout.addLayout(self.button_layout)  
    def camera_buttons(self):
        self.snap_btn = QPushButton("Snap")
        self.video_btn = QPushButton("Video")
        for i, btn in enumerate([self.snap_btn,self.video_btn], start=1):
            btn.setStyleSheet( self.parent.btn_properties)
            self.button_layout.insertWidget(i,btn)
        self.snap_btn.clicked.connect(self.camera.snap_picture)
        self.video_btn.clicked.connect(self.camera.recorder)
    def toggle_main_screen(self,show:bool):
        if show:
            self.parent.map_label.show()
            self.parent.city_label.show()
            self.parent.coordinate_label.show()
        else:
            self.parent.map_label.hide()
            self.parent.city_label.hide()
            self.parent.coordinate_label.hide()
    def log_clicked(self):
        print("test")
    def camera_clicked(self):
        # self.parent.widget_checker()

        # self.parent.map_label.hide()
        # self.camera = CAMERA_WINDOW(self.parent)
        # self.camera.camera_label.show()
        # self.parent.widget_checker()
        
        if self.parent.active_feature is None:
            self.toggle_main_screen(False)
            self.log_btn.hide()
            self.reload_btn.hide()
            self.camera.start_camera()
            self.camera_buttons()
            self.parent.widget_checker()
            self.parent.active_feature = self.camera.camera_label

            self.parent.active_feature.show()

        else:
            print("stoping")
            self.camera.stop_camera()
            self.parent.layout.removeWidget(self.parent.active_feature)
            self.parent.active_feature.hide()
            self.parent.widget_checker()
            self.snap_btn.hide()
            self.video_btn.hide()
            self.toggle_main_screen(True)
            self.log_btn.show()
            self.reload_btn.show()
            self.parent.active_feature =None
            print(self.parent.active_feature)
            return
    def galerry_btn_clicked(self):
        self.db = DB()

        # Scroll Area
        self.scroll = QScrollArea()
        self.scroll.setWidgetResizable(True)

        # Gallery Container Widget
        self.gallery_widget = QWidget()
        self.gallery_layout = QGridLayout(self.gallery_widget)
        self.gallery_layout.setContentsMargins(0, 0, 0, 0)
        self.gallery_layout.setSpacing(5)
        self.gallery_layout.setVerticalSpacing(5)


        # Add images to gallery
        row = 0
        col = 0
        for i, img_path in enumerate(self.db.get_all_from_album()):
            label = ClickableLabel()
            
            # Adjust image size — e.g. 150x150 max
            pixmap = QPixmap(img_path).scaled(235, 235, Qt.KeepAspectRatio, Qt.SmoothTransformation)
            label.setPixmap(pixmap)
            label.setFixedSize(pixmap.size())
            
            label.clicked.connect(partial(self.show_preview, img_path))

            # Add to grid layout
            self.gallery_layout.addWidget(label, row, col)

            # Go to next column or row
            col += 1
            if col >= 2:  # ← two per row
                col = 0
                row += 1
        # Attach layout to scroll area
        self.scroll.setWidget(self.gallery_widget)
        # Add to main layout once
        if self.parent.active_feature is None:
            self.parent.active_feature = self.scroll
            self.toggle_main_screen(False)
            self.parent.layout.insertWidget(1,self.scroll)
            return
        else:
            self.parent.active_feature.hide()
            self.parent.layout.removeWidget(self.parent.active_feature)
            self.parent.active_feature =None
            self.toggle_main_screen(True)
            return

        

    def show_preview(self, img_path):
        dlg = QDialog()
        dlg.setWindowTitle("Preview")
        dlg_layout = QVBoxLayout()
        dlg.setLayout(dlg_layout)

        pixmap = QPixmap(img_path).scaled(600, 600, Qt.KeepAspectRatio, Qt.SmoothTransformation)
        preview_label = QLabel()
        preview_label.setPixmap(pixmap)
        preview_label.setAlignment(Qt.AlignCenter)

        close_btn = QPushButton("Close")
        close_btn.clicked.connect(dlg.accept)

        dlg_layout.addWidget(preview_label)
        dlg_layout.addWidget(close_btn)
        dlg.exec()

class ClickableLabel(QLabel):
    clicked =pyqtSignal()
    def __init__(self ,parent =None):
        super(ClickableLabel,self).__init__(parent)
    def mousePressEvent(self,event):
        self.clicked.emit()



        
if __name__ == "__main__":
    app = APP()
    # city = LOCATION()


    button = BUTTONS(app)
    app.run()
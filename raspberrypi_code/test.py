from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout
from picamera2 import Picamera2
from picamera2.previews.qt import QPicamera2

picam2 = Picamera2()

# Configure with higher res for better quality
config = picam2.create_preview_configuration(main={"size": (1280, 720)})
picam2.configure(config)
picam2.set_controls({"Contrast": 50})
app = QApplication([])
window = QWidget()
layout = QVBoxLayout(window)

preview = QPicamera2(picam2, width=1280, height=720, keep_ar=False)
layout.addWidget(preview)

window.setWindowTitle("PiCam2 Preview")
window.setLayout(layout)
window.resize(1920, 1080)

picam2.start()
window.show()
app.exec()

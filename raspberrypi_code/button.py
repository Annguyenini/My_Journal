import RPi.GPIO as GPIO
from camera import Camera
import time
class Button:
    def __init__ (self):
        GPIO.setmode(GPIO.BOARD)  # Use physical pin numbers
        GPIO.setup(7, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        self.camera = Camera()
    # def is_display_on(self):
        
    def on_click(self):
        
        # gps = GPS()
        try:
            while True:
                if GPIO.input(7) == GPIO.LOW:  # LOW = button pressed
                    print("Button pressed!")
                    self.camera.preview()
                    self.camera.start_camera()
                    # camera.take_picture()
                    # print(gps.get_data_fromDB("2025_07_21_003035"))
                    time.sleep(0.5)  # debounce delay
                time.sleep(0.1)
        except KeyboardInterrupt:
            print("Exiting...")
            GPIO.cleanup()

button = Button()
button.on_click()


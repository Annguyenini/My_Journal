class GPS:
    def __init__(self):
        self.lat =0.0005
        self.lng =0.0001
    def get_coordinate(self):
        # Simulate movement
        self.lat += 0.001
        self.lng += 0.001
        return (self.lat, self.lng)
    def get_current_city(self):
        return ("testcity")
    
    def start_thread(self):
        print ("thread active in test")

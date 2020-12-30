"""A quick example for using prucam"""

from prucam import Camera, PRU

pru = PRU()
pru.start()
c = Camera()

c.capture(dir_path=".", ext=".jpg")

pru.stop()

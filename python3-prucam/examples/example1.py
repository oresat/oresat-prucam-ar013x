"""A quick example for using prucam"""

from prucam import Camera, PRU

pru = PRU()
pru.start()
c = Camera()
print(c.blue_gain)

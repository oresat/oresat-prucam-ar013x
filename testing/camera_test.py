#!/usr/bin/python3

import os
import io
import cv2
import numpy as np

cols = 1280
rows = 960
pixels = cols * rows

path="/dev/prucam"

# open up the prucam char device
fd = os.open(path, os.O_RDWR)
fio = io.FileIO(fd, closefd = False)

# make buffer to read into
imgbuf = bytearray(pixels)

# read from prucam into buffer
fio.readinto(imgbuf)

# read image bytes into ndarray
img = np.frombuffer(imgbuf, dtype=np.uint8).reshape(rows, cols)

# do bayer color conversion. For monochrome/raw image, comment out
img = cv2.cvtColor(img, cv2.COLOR_BayerBG2BGR)

# json encode image
ok, img = cv2.imencode('.jpg', img)
if not(ok):
    raise BaseException("encoding error")

# write image
with open('capt.jpg', 'wb') as f:
    f.write(img)

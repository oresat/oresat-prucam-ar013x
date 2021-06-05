#!/usr/bin/python3

import os
import io
import sys
import cv2
import numpy as np

cols = 1280
rows = 1024
pixel_bytes = cols * rows * 2

path="/dev/prucam"

# open up the prucam char device
fd = os.open(path, os.O_RDWR)
fio = io.FileIO(fd, closefd = False)

# make buffer to read into
imgbuf = bytearray(pixel_bytes)

# read from prucam into buffer
fio.readinto(imgbuf)

# write buffer out to file
out = open('img.buf', 'wb')
out.write(imgbuf)

# read image bytes into ndarray
img = np.frombuffer(imgbuf, dtype=np.uint16).reshape(rows, cols)

# do bayer color conversion. For monochrome/raw image, comment out
#img = cv2.cvtColor(img, cv2.COLOR_BayerBG2BGR)

# json encode image
ok, img = cv2.imencode('.bmp', img, params=[cv2.CV_16U])
if not(ok):
    raise BaseException("encoding error")

# write image
with open('capt.bmp', 'wb') as f:
    f.write(img)

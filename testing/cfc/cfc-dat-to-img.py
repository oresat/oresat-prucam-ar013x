#!/usr/bin/python3

import os
import io
import sys
import cv2
import numpy as np

cols = 1280
rows = 1024
pixel_bytes = cols * rows * 2

# input file is first arg
path=sys.argv[1]

fd = os.open(path, os.O_RDWR)
fio = io.FileIO(fd, closefd = False)

# make buffer to read into
imgbuf = bytearray(pixel_bytes)

# read from prucam into buffer
fio.readinto(imgbuf)

# read image bytes into ndarray
img = np.frombuffer(imgbuf, dtype=np.uint16).reshape(rows, cols)

# flip the image vertically because it is read upside down
img = cv2.flip(img, 0)

# json encode image
ok, img = cv2.imencode('.png', img, params=[cv2.CV_16U])
if not(ok):
    raise BaseException("encoding error")

# write image
with open('capt.png', 'wb') as f:
    f.write(img)

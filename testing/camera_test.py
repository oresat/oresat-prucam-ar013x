#!/usr/bin/python3

import os
import io
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

# create an empty array of uint16 large enough to fit the image
img = np.zeros(rows * cols, np.uint16)

arr = []

for i in range(0, len(imgbuf), 2):
    # the PIRT1280 pixels are 14 bits, split along 2 bytes, 7 bits each. The
    # MSB comes first (big-endian), so take the high order byte, shift it one 
    # bit to the right, and then OR it with the low order byte
    idx = i>>1
    result = np.uint16((imgbuf[i] << 7) | imgbuf[i+1])
    #np.insert(img, idx, result)
    arr.append(result)

    if i % 100000 == 0:
        print("progress: ", i)

    #print("BYTE[{}]: {}".format(i, hex(imgbuf[i])))
    #print("BYTE[{}]: {}".format(i+1, hex(imgbuf[i+1])))
    #print("RES: ", result, type(result))


img = np.array(arr)

out = open('imgconv.buf', 'wb')
out.write(img.tobytes())


# read image bytes into ndarray
#img = np.frombuffer(imgbuf, dtype=dt).reshape(rows, cols)
img = img.reshape(rows, cols)

# do bayer color conversion. For monochrome/raw image, comment out
#img = cv2.cvtColor(img, cv2.COLOR_BayerBG2BGR)

# json encode image
ok, img = cv2.imencode('.bmp', img, params=[cv2.CV_16U])
if not(ok):
    raise BaseException("encoding error")

# write image
with open('capt.bmp', 'wb') as f:
    f.write(img)

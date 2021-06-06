#!/usr/bin/python3

import os
import io
from flask import Flask, request, Response
from time import sleep
import numpy as np
import cv2
import logging

api = Flask(__name__)

# constants
cols = 1280
rows = 1024
pixel_bytes = cols * rows * 2
path="/dev/prucam"

@api.route("/<filename>")
def get_image(filename):

    # some browsers will make favicon requests, ignore them
    if "favicon" in filename:
        return Response()

    if "integration" in request.args:
        itg_time = request.args.get("integration")
        logging.info("setting 'integration time to {}".format(itg_time))

    fd = os.open(path, os.O_RDWR)
    fio = io.FileIO(fd, closefd = False)

    # make buffer to read into
    imgbuf = bytearray(pixel_bytes)

    # read from prucam into buffer
    fio.readinto(imgbuf)

    # read image bytes into ndarray
    img = np.frombuffer(imgbuf, dtype=np.uint16).reshape(rows, cols)

    # flip the image because it is read upside down
    img = cv2.flip(img, 0)
    img = cv2.flip(img, 1)

    # invert image colors to look normal
    img = cv2.bitwise_not(img)

    # parse the extension from the filename
    ext = os.path.splitext(filename)[1].lower()

    # try to encode the image with the provided image extension
    ok, encoded = cv2.imencode(ext, img, params=[cv2.CV_16U])
    if not(ok):
        print("{} encode error".format(ext))
        return Response(status=400)

    # return the encoded buffer bytes in the request
    return Response(encoded.tobytes(), mimetype='text/plain')

if __name__ == "__main__":
    api.run(host='0.0.0.0', port=5000)

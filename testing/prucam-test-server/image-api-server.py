#!/usr/bin/python3

import os
import io
from flask import Flask, request, Response
from time import sleep
import numpy as np
import cv2

api = Flask(__name__)

# constants
cols = 1280
rows = 960
pixels = cols * rows
path="/dev/prucam"
prucam_sysfs = "/sys/devices/virtual/pru/prucam/"

# possible camera settings
cam_settings =[
        "coarse_time",
        "fine_time",
        "global_gain",
        "analog_gain",
        "context",
        "digital_binding",
        "red_gain",
        "blue_gain",
        "green1_gain",
        "green2_gain",
        "x_size",
        "y_size",
        "y_odd_inc",
        "frame_len_lines",
        ]

@api.route("/<filename>")
def get_image(filename):

    # some browsers will make favicon requests, ignore them
    if "favicon" in filename:
        return Response()

    # loop through all the possible camera settings and set them if
    # they are specified as query params
    param_changed = False
    for param in cam_settings:
        # if this seting is in the query params, set it
        if param in request.args:
            # create the sysfs path and write it
            param_path = os.path.join(prucam_sysfs, param)
            with open(param_path, 'r+') as f:
                val = f.read()
                if not(request.args.get(param) in val):                    
                    print("Changing {} from {} to {}".format(param, val.rstrip(), request.args.get(param)))
                    f.write(request.args.get(param))
                    param_changed = True

    # If a param changed, sleep a moment. This seems required for the settings to 
    # take but we should come back to this because they should be more immediate
    if param_changed:
        sleep(0.2)

    # open up the prucam char device
    fd = os.open(path, os.O_RDWR)
    fio = io.FileIO(fd, closefd = False)

    # make buffer to read into
    img = bytearray(pixels)

    # read from prucam into buffer
    fio.readinto(img)
    
    # make buffer to read into
    imgbuf = bytearray(pixels)

    # read from prucam into buffer
    fio.readinto(imgbuf)
    
    # convert to ndarray and reshape to cols/rows
    img = np.frombuffer(imgbuf, dtype=np.uint8).reshape(rows, cols)

    # do bayer color conversion if bayer=true in request params
    if "bayer" in request.args and "true" in request.args.get("bayer"):
        img = cv2.cvtColor(img, cv2.COLOR_BayerBG2BGR)

    # parse the extension from the filename
    ext = os.path.splitext(filename)[1].lower()

    # try to encode the image with the provided image extension
    ok, encoded = cv2.imencode(ext, img)
    if not(ok):
        print("{} encode error".format(ext))
        return Response(status=400)

    # return the encoded buffer bytes in the request 
    return Response(encoded.tobytes(), mimetype='text/plain')

if __name__ == "__main__":
    api.run(host='0.0.0.0', debug=True, port=5000)

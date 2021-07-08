#!/usr/bin/python3

import os
import io
from flask import Flask, request, Response
from time import sleep
import numpy as np
import cv2
import logging
from cfc_tec import ctrl
from pirt1280 import pirt1280

api = Flask(__name__)

log = logging.getLogger()
log.setLevel(logging.INFO)

# constants
cols = 1280
rows = 1024
pixel_bytes = cols * rows * 2
path="/dev/prucam"


@api.route("/tec/disable", methods = ['POST'])
def tec_disable():
    ctrl.stop()
    return Response(status=204)

@api.route("/tec/temperature", methods = ['GET'])
def get_tec_temp():
    temp = ctrl.get_temp()
    return Response(response=str(temp), status=200)

@api.route("/tec/temperature", methods = ['POST'])
def set_tec_temp():

    temp = request.args.get("temperature")
    if not temp:
        return Response(response='no temperature specified\n', status=400)

    temp = int(temp)

    if temp < -30 or temp > 40:
        return Response(response='temperature must be between -30C and 40C\n', status=400)

    ctrl.set_temp(temp)
    return Response(status=204)


@api.route("/tec/enable", methods = ['POST'])
def tec_enable():
    # parse the temperature from the query params
    temp = request.args.get("value")
    if not temp:
        return Response(response='no temperature specified\n', status=400)

    temp = int(temp)

    if temp < -30 or temp > 40:
        return Response(response='temperature must be between -30C and 40C\n', status=400)


    print("TEMPERATURE: ", temp)
    
    try:
        ctrl.start(20)
    except Exception as e:
        return Response(response='error starting TEC controller: ' + str(e), status=400)

    return Response(status=204)


@api.route("/pirt/enable", methods = ['POST'])
def pirt_enable():
    pirt1280().enable()
    return Response(status=204)

@api.route("/pirt/disable", methods = ['POST'])
def pirt_disable():
    pirt1280().disable()
    return Response(status=204)

@api.route("/pirt/integration", methods = ['POST'])
def pirt_integration():
    # parse the integration from the query params
    integration = request.args.get("value")
    if not integration:
        return Response(response='no integration specified\n', status=400)

    integration = float(integration)
    pirt1280().set_integration(integration)

    return Response(status=204)




@api.route("/<filename>")
def get_image(filename):

    # some browsers will make favicon requests, ignore them
    if "favicon" in filename:
        return Response()

    if "integration" in request.args:
        itg_time = request.args.get("integration")
        logging.info("setting integration time to {}".format(itg_time))

    fd = os.open(path, os.O_RDWR)
    fio = io.FileIO(fd, closefd = False)

    # make buffer to read into
    imgbuf = bytearray(pixel_bytes)

    # read from prucam into buffer
    fio.readinto(imgbuf)

    # write raw buffer out to file
    out = open('img.buf', 'wb')
    out.write(imgbuf)

    # read image bytes into ndarray
    img = np.frombuffer(imgbuf, dtype=np.uint16).reshape(rows, cols)
    
    # pixel values are only 14 bits, but opencv expects 16 bits, so shift each
    # value 2 bits to the left
    img = img << 2

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

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
prucam_sysfs_ctx_settings = "/sys/devices/virtual/pru/prucam/context_settings/"
prucam_sysfs_ae_settings = "/sys/devices/virtual/pru/prucam/auto_exposure_settings"

# possible camera context settings
ctx_settings = [
        "analog_gain",
        "blue_gain",
        "coarse_time",
        "context",
        "digital_binning",
        "fine_time",
        "frame_len_lines",
        "global_gain",
        "green1_gain",
        "green2_gain",
        "red_gain",
        "x_size",
        "y_odd_inc",
        "y_size",
        ]

# auto exposure settings
ae_settings = [
        "ae_ag_en",
        "ae_ag_exposure_hi",
        "ae_ag_exposure_lo",
        "ae_damp_gain",
        "ae_damp_max",
        "ae_damp_offset",
        "ae_dark_cur_thresh",
        "ae_dg_en",
        "ae_enable",
        "ae_luma_target",
        "ae_max_ev_step",
        "ae_max_exposure",
        "ae_min_ana_gain",
        "ae_min_ev_step",
        "ae_min_exposure",
        "ae_roi_x_size",
        "ae_roi_x_start_offset",
        "ae_roi_y_size",
        "ae_roi_y_start_offset",
        ]

# possible bayer mappings
bayer_mappings = {
        "BG2BGR": cv2.COLOR_BayerBG2BGR,
        "GB2BGR": cv2.COLOR_BayerGB2BGR,
        "RG2BGR": cv2.COLOR_BayerRG2BGR,
        "GR2BGR": cv2.COLOR_BayerGR2BGR,
        "BG2RGB": cv2.COLOR_BayerBG2RGB,
        "GB2RGB": cv2.COLOR_BayerGB2RGB,
        "RG2RGB": cv2.COLOR_BayerRG2RGB,
        "GR2RGB": cv2.COLOR_BayerGR2RGB
        }

def settings_from_path_params(request, settings, path):
    # loop through all the possible camera settings and set them if
    # they are specified as query params
    param_changed = False
    for param in settings:
        # if this seting is in the query params, set it
        if param in request.args:
            # create the sysfs path and write it
            param_path = os.path.join(path, param)
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

@api.route("/<filename>")
def get_image(filename):

    # some browsers will make favicon requests, ignore them
    if "favicon" in filename:
        return Response()

    # set sysfs settings from request params
    settings_from_path_params(request, ctx_settings, prucam_sysfs_ctx_settings)
    settings_from_path_params(request, ae_settings, prucam_sysfs_ae_settings)

    # open up the prucam char device
    fd = os.open(path, os.O_RDWR)
    fio = io.FileIO(fd, closefd = False)

    # make buffer to read into
    imgbuf = bytearray(pixels)

    # read from prucam into buffer
    fio.readinto(imgbuf)

    # convert to ndarray and reshape to cols/rows
    img = np.frombuffer(imgbuf, dtype=np.uint8).reshape(rows, cols)

    # do bayer color conversion if bayer param given
    if "bayer" in request.args:
        try:
            img = cv2.cvtColor(img, bayer_mappings[request.args.get("bayer")])
        except:
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

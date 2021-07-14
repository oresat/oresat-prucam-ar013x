#!/usr/bin/python3

import requests
import os
import time

default_cfc_host = "http://192.168.6.2:5000"

# override the default host if specified via env var
cfc_host = os.getenv('CFC_HOST')
if cfc_host is None:
    cfc_host = default_cfc_host

# get_test_image executes a routine to enable and configure the PIRT1280, 
# enable the TEC at a setpoint, and retrieve an image
def get_test_image():
    # enable the PIRT1280
    requests.put(cfc_host + "/pirt/enable")

    # wait a moment for the sensor to start
    time.sleep(0.2)

    # set the integration time to 10ms (0.01s)
    requests.put(cfc_host + "/pirt/setting/integration?value=0.01")

    # TEC temperature setpoint in celcius
    tec_setpoint = 28

    # set the TEC temperature to 10C (defined above)
    requests.put(cfc_host + "/tec/enable?temperature=" + str(tec_setpoint))

    # continually request the temperature of the TEC, waiting for it to get below the setpoint
    tec_temp = float(requests.get(cfc_host + "/tec/temperature").text)
    while tec_temp > tec_setpoint: 
        print("TEC at " + str(tec_temp) + "C, waiting...")
        time.sleep(1)
        tec_temp = float(requests.get(cfc_host + "/tec/temperature").text)

    print("saving image.png")
    # get the image as a PNG!
    resp = requests.get(cfc_host + "/image.png")

    # write the png to a file
    with open("image.png", 'wb') as f:
        f.write(resp.content)

    print("saving image.raw")
    # get the image as a raw image buffer
    resp = requests.get(cfc_host + "/image?raw=true")

    # write the raw buffer to a file
    with open("image.bin", 'wb') as f:
        f.write(resp.content)

    # disable the TEC
    requests.put(cfc_host + "/tec/disable")

    # disable the PIRT1280
    requests.put(cfc_host + "/pirt/enable")

def main():
    try:
        get_test_image()
    except Exception as e:
        print("Exception: ", e)

    # always disable the TEC and PIRT, even on error
    requests.put(cfc_host + "/tec/disable")
    requests.put(cfc_host + "/pirt/enable")


if __name__ == "__main__":
    main()

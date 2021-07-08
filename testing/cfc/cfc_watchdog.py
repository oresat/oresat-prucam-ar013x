#!/usr/bin/python3

# this program run in the background and makes sure the TEC controller is not
# hung by checkinmg the timestamp it updates frequently

import gpio
from datetime import datetime
import time

watchdog_file = '/tmp/tec.watchdog'
tec_gpio = 88

# if timestamp older than 5 seconds, disable the TEC
timeout = 5

# check_timestamp reads the last timestamp from the TEC controller and raises
# an error is the timestamp is older than the timeout
def check_timestamp():
    with open(watchdog_file, 'r') as f:
        ts_str = f.read()
        ts = int(ts_str)

        print("last TEC controller timestamp: ", ts)
        last = datetime.utcfromtimestamp(ts)

        if (datetime.now() - last).total_seconds() > timeout:
            raise Exception("TEC timestamp is stale!")

# run forever
while True:
    # on an interval, check the timestamp and disable the GPIO on any error
    time.sleep(timeout)
    try:
        check_timestamp()
    except Exception as e:
        print("error checking TEC timestamp, disabling: ", e)
        gpio.setup(tec_gpio, gpio.OUT)
        gpio.set(tec_gpio, 0)


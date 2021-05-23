#!/usr/bin/python3

import math
import signal
import time
import gpio
import logging
import os
from simple_pid import PID
import json
from datetime import datetime 
import threading
import sys
import traceback

logging.getLogger().setLevel(logging.DEBUG)

tec_gpio = 88

adc_path = '/sys/bus/iio/devices/iio:device0/in_voltage0_raw'

watchdog_file = '/tmp/tec.watchdog'

adc_file = None

# resistance of the upper resistor in the divider
r1 = 10000

# resistance of the NTC at 25C
r25c = 10000

# TODO should this be 4095?
adc_steps = 4096
vin = 1.8

# steinhart coefficients per Vishay website for part NTCS0805E3103FMT
# NOTE: don't take the equations off the site it self, export/download the
# data for the specific part as there are more specifics there.
# https://www.vishay.com/thermistors/ntc-rt-calculator/
sh_a = 0.003354016434680530000
sh_b = 0.000286451700000000000
sh_c = 0.000003252255000000000
sh_d = 0.000000045945010000000

# __TecController allows control of the PIRT1280 TEC controller. However, this 
# object should NEVER be instantiated anywhere but the singleton at the
# bottom of this module
class __TecController:

    def res_to_temp_celcius(self, res):
        # Per the steinhart/hart equation where A-D are the steinhart coefficients,
        # R25 is the NTC resistance at 25C(10k), and RT is the NTC resistance
        # T=1/(A1+B1*LN(RT/R25)+C1*LN(RT/R25)^2+D1*LN(RT/R25)^3)-273.15
        temp_k = 1 / (sh_a + (sh_b * math.log(res/r25c)) + (sh_c * math.pow(math.log(res/r25c), 2)) + (sh_d * math.pow(res/r25c, 3)))
        temp_c = temp_k - 273.15
        return temp_c

    def _get_temp(self):
        with open(adc_path, 'r') as f:
            val = f.read()
            val = int(val)
            #print("ADC raw: ", val)

            # convert the adc value 0 - 4096 to volts
            vout = (val / adc_steps) * vin

            #print("Vout: ", vout, "volts")

            # The 10k NTC is part of a voltage divider with a 10k resistor between 
            # 1.8v and Vout and the NTC between Vout and ground. Thus, the equation is:
            #
            # Vout = 1.8v * (NTC / (10k + NTC))
            #
            # We know volts and need to solve for NTC resistance, which is:
            #
            # NTC = (10k * Vout) / (1.8v - Vout)
            
            res = (r1 * vout) / (vin - vout)

            #print("NTC: ", res, "ohms")

            temp = self.res_to_temp_celcius(res)
            #print("Temp: ", temp)

            return temp

    def get_temp(self):
            samples = 10
            samples_sum = 0
            for i in range(samples):
                samples_sum += self._get_temp()

            avg = samples_sum / samples
            return avg


    # pid loop params
    pid_p = 0.5
    pid_i = 0.0
    pid_d = 0.1

    # run the PID controller loop every 250ms
    pid_period = 0.25

    def __init__(self):
        # try and catch all exitting signals and disable the TEC. 
        # WARNING this may overwrite signal handlers of caller
        signal.signal(signal.SIGINT, self._safe_exit)
        signal.signal(signal.SIGTERM, self._safe_exit)
        signal.signal(signal.SIGQUIT, self._safe_exit)
        
        self.lock = threading.Lock()

        self.pid =  PID(self.pid_p, self.pid_i, self.pid_d)

        # TEC controller starts in the off state
        self.enabled = False  # if the controller is enabled/disabled
        self.tec_state = 0 # is the TEC is currently on/off(while controller is enabled)
        gpio.setup(tec_gpio, gpio.OUT)
        self._disable_tec()

        # timer holds the period thread object
        self.timer = None

    def _safe_exit(self, *args):
        # try and disable the TEC manually before quitting
        gpio.setup(tec_gpio, gpio.OUT)
        gpio.set(tec_gpio, 0)
        print("disabling TEC before exit")
        sys.exit(1)

    def start(self, setpoint):

        # first, get the lock so it can only be started once
        got_lock = self.lock.acquire(False)
        if not got_lock:
            raise Exception("controller already started")

        # save the setpoint to the PID controller
        self.pid.setpoint = setpoint

        # set the controller as enabled
        self.enabled = True

        # enable the timer thread that actually executes the PID loop
        self.timer = threading.Timer(self.pid_period, self._run)
        self.timer.start()


    def set_temp(self, setpoint):
        self.pid.setpoint = setpoint

    def _disable_tec(self):
        gpio.set(tec_gpio, 0)
        self.tec_state = 0


    def _enable_tec(self):
        gpio.set(tec_gpio, 1)
        self.tec_state = 1


    def stop(self):
        self.enabled = False

        # attempt to disable the GPIO here as well
        self._disable_tec()

        # attempt to stop the timer as well
        try:
            self.timer.cancel()
        except:
            logging.error("error stopping timer")

        # finally, release the lock
        try:
            self.lock.release()
        except:
            pass

    def _run(self):

        with open(watchdog_file, 'w') as f:
            f.write(str(int(datetime.now().timestamp())))

        # is the controller is not enabled, drive the gpio low and quit
        if not self.enabled:
            self._disable_tec()
            logging.info("exitting TEC control thread")
            return

        # if we are here then the controller is enabled, so schedule the next 
        # periodic run
        self.timer = threading.Timer(self.pid_period, self._run)
        self.timer.start()

        # sample the temp and get the PID correction
        current_temp = self.get_temp()
        diff = self.pid(current_temp)

        if diff < 0:
            if self.tec_state == 0:
                logging.debug("enabled TEC")
                self._enable_tec()
        else:
            if self.tec_state == 1:
                logging.debug("disabling TEC")
                self._disable_tec()
            
        msg = {
            'temp': current_temp,
            'pid': diff,
            'state': self.tec_state,
            'setpoint': self.pid.setpoint
        }
        b = json.dumps(msg)
        logging.debug(b)

# the SINGLE, GLOBAL TEC controller. The object should never be instantiated 
# anywhere else but here
ctrl = __TecController()

if __name__ == "__main__":
    try:
        # little test program
        ctrl.start(10)
        time.sleep(2000)
        ctrl.set_temp(15)
        time.sleep(20)
        ctrl.set_temp(5)
        time.sleep(20)
        ctrl.stop()

    except:
        try:
            ctrl.stop()
        except:
            logging.error("error stopping controller")

        # try and drive the GPIO low manually as well
        gpio.setup(tec_gpio, gpio.OUT)
        gpio.set(tec_gpio, 0)
        traceback.print_exc()

        print("exitting...")



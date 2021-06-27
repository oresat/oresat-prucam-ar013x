#!/usr/bin/python3

import math
import time
import gpio
import logging

logging.getLogger().setLevel(logging.INFO)

adc_path = '/sys/bus/iio/devices/iio:device0/in_voltage0_raw'

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

def res_to_temp_celcius(res):
    # Per the steinhart/hart equation where A-D are the steinhart coefficients,
    # R25 is the NTC resistance at 25C(10k), and RT is the NTC resistance
    # T=1/(A1+B1*LN(RT/R25)+C1*LN(RT/R25)^2+D1*LN(RT/R25)^3)-273.15
    temp_k = 1 / (sh_a + (sh_b * math.log(res/r25c)) + (sh_c * math.pow(math.log(res/r25c), 2)) + (sh_d * math.pow(res/r25c, 3)))
    temp_c = temp_k - 273.15
    return temp_c

def get_temp_celcius():
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

        temp = res_to_temp_celcius(res)
        #print("Temp: ", temp)

        return temp

def sample():
        samples = 10
        samples_sum = 0
        for i in range(samples):
            samples_sum += get_temp_celcius()

        avg = samples_sum / samples
        return avg

from simple_pid import PID

tec_gpio = 88

import paho.mqtt.client as paho
import json

def main():

    setpoint = 0
    mqtt = paho.Client()
    mqtt.connect("192.168.1.115", 1883, 2)

    pid = PID(15, 0.05, 0.2, setpoint=setpoint)
    gpio.setup(tec_gpio, gpio.OUT)
    gpio.set(tec_gpio, 0)
    state = 0

    temp = sample()
    while True:
        diff = pid(temp)
        print("Temp: ", temp, "C")
        print("PID: ", diff)
        print("======")

        msg = {
            'temp': temp,
            'state': state,
            'setpoint': setpoint
        }

        mqtt.publish("tecmetrics999", json.dumps(msg), qos=0)

        if diff < 1:
            print("ENABLING TEC")
            gpio.set(tec_gpio, 1)
            state = 1
        else:
            print("DISABLING TEC")
            gpio.set(tec_gpio, 0)
            state = 0
            

        temp = sample()

        time.sleep(0.1)

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print("DISABLE ON EXIT: ", e)
        gpio.setup(tec_gpio, gpio.OUT)
        gpio.set(tec_gpio, 0)



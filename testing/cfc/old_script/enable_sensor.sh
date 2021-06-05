#!/bin/bash
sudo su -c 'echo out > /sys/class/gpio/gpio86/direction'
sudo su -c 'echo 1 > /sys/class/gpio/gpio86/value'

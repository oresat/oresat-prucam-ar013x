#!/usr/bin/python3

import spidev
import time

spi = spidev.SpiDev()

spi.open(1, 1) # /dev/spidev1.1

# 100KHz
spi.max_speed_hz = 100000

rws0 = 0x0a
rws1 = 0x0b
cws0 = 0x04
cws1 = 0x05


# read RWS0
spi.writebytes([rws0])
read = spi.readbytes(1)
print("RWS0: ", hex(read[0]), read[0])

# read RWS1
spi.writebytes([rws1])
read = spi.readbytes(1)
print("RWS1: ", hex(read[0]), read[0])

# read CWS0
spi.writebytes([cws0])
read = spi.readbytes(1)
print("CWS0: ", hex(read[0]), read[0])

# read CWS1
spi.writebytes([cws1])
read = spi.readbytes(1)
print("CWS1: ", hex(read[0]), read[0])


spi.close()

#!/usr/bin/python3

import spidev
import time

spi = spidev.SpiDev()

spi.open(1, 1) # /dev/spidev1.1

# 100KHz
spi.max_speed_hz = 100000

it0 = 0x0e
it1 = 0x0f
it2 = 0x10
it3 = 0x11

reg_wr = 0x40
# 159EC ~ 88556
spi.writebytes([it0 | reg_wr, 0xff])
spi.writebytes([it1 | reg_wr, 0x00])
spi.writebytes([it2 | reg_wr, 0x08])
spi.writebytes([it3 | reg_wr, 0x0])

# wait a sec for it to apply
time.sleep(0.25)

# read back 
spi.writebytes([it0])
read = spi.readbytes(1)
print("it0: ", hex(read[0]), read[0])

spi.writebytes([it1])
read = spi.readbytes(1)
print("it1: ", hex(read[0]), read[0])

spi.writebytes([it2])
read = spi.readbytes(1)
print("it2: ", hex(read[0]), read[0])

spi.writebytes([it3])
read = spi.readbytes(1)
print("it3: ", hex(read[0]), read[0])

spi.close()

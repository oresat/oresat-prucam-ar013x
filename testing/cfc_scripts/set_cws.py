#!/usr/bin/python3

import spidev
import time

spi = spidev.SpiDev()

spi.open(1, 1) # /dev/spidev1.1

# 100KHz
spi.max_speed_hz = 100000
reg_wr = 0x40
cws0 = 0x04
cws1 = 0x05

# width = 1310 = 0x51E
#
spi.writebytes([cws0 | reg_wr, 0x1E ])
spi.writebytes([cws1 | reg_wr, 0x05 ])

# read CWS0
spi.writebytes([cws0])
read = spi.readbytes(1)
print("CWS0: ", hex(read[0]), read[0])

# read CWS1
spi.writebytes([cws1])
read = spi.readbytes(1)
print("CWS1: ", hex(read[0]), read[0])


spi.close()

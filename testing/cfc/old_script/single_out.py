#!/usr/bin/python3

import spidev
import time

spi = spidev.SpiDev()

spi.open(1, 1) # /dev/spidev1.1

# 100KHz
spi.max_speed_hz = 100000

conf1_rd = 0x27
conf1_wr = 0x67

# read CONF1
spi.writebytes([conf1_rd])
read = spi.readbytes(1)
print("CONF1: ", hex(read[0]), read[0])

# modify and write CONF1
new = read[0] & 0x3F
print("MOD CONF1:", hex(new), new)
spi.writebytes([conf1_wr, new])

# wait a sec for it to apply
time.sleep(0.25)

# read back CONF1
spi.writebytes([conf1_rd])
read = spi.readbytes(1)
print("FINAL CONF1: ", hex(read[0]), read[0])

spi.close()

#!/usr/bin/python3

import spidev
import time

spi = spidev.SpiDev()

spi.open(1, 1) # /dev/spidev1.1

# 100KHz
spi.max_speed_hz = 100000

ft0 = 0x12
ft1 = 0x13
ft2 = 0x14
ft3 = 0x15

reg_wr = 0x40

spi.writebytes([ft0 | reg_wr, 0x41])
spi.writebytes([ft1 | reg_wr, 0x83])
spi.writebytes([ft2 | reg_wr, 0x1d])
spi.writebytes([ft3 | reg_wr, 0x0])

# wait a sec for it to apply
time.sleep(0.25)

# read back 
spi.writebytes([ft0])
read = spi.readbytes(1)
print("ft0: ", hex(read[0]), read[0])

spi.writebytes([ft1])
read = spi.readbytes(1)
print("ft1: ", hex(read[0]), read[0])

spi.writebytes([ft2])
read = spi.readbytes(1)
print("ft2: ", hex(read[0]), read[0])

spi.writebytes([ft3])
read = spi.readbytes(1)
print("ft3: ", hex(read[0]), read[0])

spi.close()

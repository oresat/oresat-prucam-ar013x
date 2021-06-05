#!/usr/bin/python3

import spidev
import time
import sys

refclk_cycle = 1 / 16000000 # 16MHz period

# row params
cws = 1400
dummy_pixels = 8
sync_pixels = 2
bytes_per_pixel = 2
rows = 1024
pix_clks_per_refclk = 4 # 4 64MHz clocks per 16MHZ refclk 
readout_refclks = (((cws + dummy_pixels + sync_pixels) * bytes_per_pixel) * rows) / pix_clks_per_refclk

readout_margin = 1.1 # amount to increase readout time for safety

print("READOUT TIME(s): ", readout_refclks * refclk_cycle)

# the first argument is a decimal number of seconds for the integration time
intr_seconds = float(sys.argv[1])

print("INTEGRATION TIME(s): ", intr_seconds)

intr_refclks = intr_seconds / refclk_cycle

print("INTEGRATION REFCLKS: ", int(intr_refclks))

# validate the integration time
if intr_refclks < 512:
    raise Exception("minimum integration is 512 REFCLKs")

# calculate the number of refclks in a frame by adding the number of refclks
# of integration plus the number of refclks it takes to read out, with a little
# bit of margin
frame_refclks = int(intr_refclks + (readout_refclks * readout_margin))

print("FRAME TIME(s): ", frame_refclks * refclk_cycle)
print("FRAME REFLCKS: ", frame_refclks)
print("FRAME REFLCKS: ", hex(frame_refclks))

# frb == frame refclk bytes
frb = frame_refclks.to_bytes(4, 'little')

print("FRAME BYTES: ", hex(frb[0]), hex(frb[1]), hex(frb[2]), hex(frb[3]))

# TOMORROW write the registers!





sys.exit(0)

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

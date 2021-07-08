#!/usr/bin/python3

import spidev
import time
import sys

spi = spidev.SpiDev()
spi.open(1, 1) # /dev/spidev1.1

# 100KHz
spi.max_speed_hz = 100000

# register addresses
it0 = 0x0e
it1 = 0x0f
it2 = 0x10
it3 = 0x11
ft0 = 0x12
ft1 = 0x13
ft2 = 0x14
ft3 = 0x15

# OR with address to write
reg_wr = 0x40

refclk = 16000000 # 16MHz
refclk_cycle = 1 / refclk # 16MHz period

# row params
cws = 1400
dummy_pixels = 8
sync_pixels = 2
bytes_per_pixel = 2
rows = 1024
pix_clks_per_refclk = 4 # 4 64MHz clocks per 16MHZ refclk 

# calculate the readout time
readout_refclks = (((cws + dummy_pixels + sync_pixels) * bytes_per_pixel) * rows) / pix_clks_per_refclk

readout_margin = 1.1 # scalar to increase readout time for safety

# the first argument is a decimal number of seconds for the integration time
intr_seconds = float(sys.argv[1])


# from the specified number of integration, get the number of integration
# refclks, rounding down the float
intr_refclks = int(intr_seconds / refclk_cycle)

# validate the integration time
if intr_refclks < 513:
    print("specified {} but minimum integration is 513 REFCLKs, settings to 513".format(intr_refclks))
    intr_refclks = 513

# calculate the number of refclks in a frame by adding the number of refclks
# of integration plus the number of refclks it takes to read out, with a little
# bit of margin
frame_refclks = int(intr_refclks + (readout_refclks * readout_margin))

# convert the frame time and integration time refclks value to little-endian 
# bytes, so the first byte is the lowest byte, which is how it should be written 
# to the register

# frb == frame refclk bytes
frb = frame_refclks.to_bytes(4, 'little')

# irb == integration reflcks bytes
irb = intr_refclks.to_bytes(4, 'little')

print("READOUT TIME(s): ", readout_refclks * refclk_cycle)
print("INTEGRATION TIME(s): ", intr_seconds)
print("FRAME TIME(s): ", frame_refclks * refclk_cycle)
print("INTEGRATION REFCLKS: ", intr_refclks)
print("FRAME REFCLKS: ", frame_refclks)
print("INTEGRATION REFCLKS(hex): ", hex(intr_refclks))
print("FRAME REFCLKS(hex): ", hex(frame_refclks))
print("INTEGRATION BYTES: ", hex(irb[0]), hex(irb[1]), hex(irb[2]), hex(irb[3]))
print("FRAME BYTES: ", hex(frb[0]), hex(frb[1]), hex(frb[2]), hex(frb[3]))

#sys.exit(0)

# write the frame time register
spi.writebytes([ft0 | reg_wr, frb[0]])
spi.writebytes([ft1 | reg_wr, frb[1]])
spi.writebytes([ft2 | reg_wr, frb[2]])
spi.writebytes([ft3 | reg_wr, frb[3]])

# write the integration time register
spi.writebytes([it0 | reg_wr, irb[0]])
spi.writebytes([it1 | reg_wr, irb[1]])
spi.writebytes([it2 | reg_wr, irb[2]])
spi.writebytes([it3 | reg_wr, irb[3]])

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

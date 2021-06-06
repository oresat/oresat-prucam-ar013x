#!/usr/bin/python3
import gpio
import spidev
import time
import logging 
import argparse

log = logging.getLogger()
log.setLevel(logging.INFO)

arg = argparse.ArgumentParser()
arg.add_argument('--enable', action='store_true', help="enable the sensor")
arg.add_argument('--disable', action='store_true', help="disable the sensor")
arg.add_argument('-v', action='store_true', help="enable debug logging")
arg.add_argument('--integration', action='store', type=float, help="set integration time in seconds")

args = arg.parse_args()

log.debug("ENABLE: {}".format(args.enable))
log.debug("DISABLE: {}".format(args.disable))
log.debug("VERBOSE: {}".format(args.v))
log.debug("INTEGRATION: {}".format(args.integration))

# SPI 100KHz
spi_hz= 100000

sensor_enable_gpio = 86

# register addresses
cws0 = 0x04
cws1 = 0x05
it0 = 0x0e
it1 = 0x0f
it2 = 0x10
it3 = 0x11
ft0 = 0x12
ft1 = 0x13
ft2 = 0x14
ft3 = 0x15
conf1 = 0x27

# OR with address to write
reg_wr = 0x40

# refclk
refclk = 16000000 # 16MHz
refclk_cycle = 1 / refclk # 16MHz period

# row params
default_cws = 1400
dummy_pixels = 8
sync_pixels = 2
bytes_per_pixel = 2
rows = 1024
pix_clks_per_refclk = 4 # 4 64MHz clocks per 16MHZ refclk 

# calculate the readout time
readout_refclks = (((default_cws + dummy_pixels + sync_pixels) * bytes_per_pixel) * rows) / pix_clks_per_refclk
readout_margin = 1.1 # scalar to increase readout time for safety

class pirt1280:

    def __init__(self):
        # init SPI
        self.spi = spidev.SpiDev()
        self.spi.open(1, 1) # /dev/spidev1.1
        self.spi.max_speed_hz = spi_hz


    def enable(self):
        log.info("enabling PIRT1280...")
        # set the enable GPIO high
        gpio.setup(sensor_enable_gpio, gpio.OUT)
        gpio.set(sensor_enable_gpio, 1)

        time.sleep(0.25)

        # TODO set ROFF register to 8 to start at row 8
        # TODO I might be able to turn down CWS or use defauth of 1279

        self.set_cws(default_cws)
        self.set_single_lane()
        self.set_integration(0.001)

    def set_single_lane(self):
        # read current CONF1 value
        self.spi.writebytes([conf1])
        read = self.spi.readbytes(1)
        log.debug("EXISTING CONF1: {} {}".format(hex(read[0]), read[0]))

        # clear bits 6 & 7 of CONF1 and write it back
        new = read[0] & 0x3F
        self.spi.writebytes([conf1 | reg_wr, new])

        # wait a sec for it to apply
        time.sleep(0.25)

        # read back CONF1
        self.spi.writebytes([conf1])
        read = self.spi.readbytes(1)
        log.debug("NEW CONF1: {} {}".format(hex(read[0]), read[0]))


    # set_cws sets the column window size
    def set_cws(self, cws):
        # convert the CWS to little-endian bytes
        b = cws.to_bytes(2, 'little')

        log.debug("Set CWS {} ({}) {} {}".format(cws, hex(cws), hex(b[0]), hex(b[1])))

        # write the register
        self.spi.writebytes([cws0 | reg_wr, b[0]])
        self.spi.writebytes([cws1 | reg_wr, b[1]])

        # wait a sec for it to apply
        time.sleep(0.25)

        # read back CWS0 and CWS1
        self.spi.writebytes([cws0])
        read = self.spi.readbytes(1)
        log.debug("CWS0: {} {}".format(hex(read[0]), read[0]))
        self.spi.writebytes([cws1])
        read = self.spi.readbytes(1)
        log.debug("CWS1: {} {}".format(hex(read[0]), read[0]))


    def set_integration(self, intr_seconds):
        log.info("setting integration time to {} seconds".format(intr_seconds))
        # from the specified number of integration, get the number of integration
        # refclks, rounding down the float
        intr_refclks = int(intr_seconds / refclk_cycle)

        # validate the integration time
        if intr_refclks < 513:
            log.warning("specified less that minimum integration of 513 REFCLKs, setting to 513".format(intr_refclks))
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

        log.debug("READOUT TIME(s): {}".format(readout_refclks * refclk_cycle))
        log.debug("INTEGRATION TIME(s): {}".format(intr_seconds))
        log.debug("FRAME TIME(s): {}".format(frame_refclks * refclk_cycle))
        log.debug("INTEGRATION REFCLKS: {}".format(intr_refclks))
        log.debug("FRAME REFCLKS: {}".format(frame_refclks))
        log.debug("INTEGRATION REFCLKS(hex): {}".format(hex(intr_refclks)))
        log.debug("FRAME REFCLKS(hex): {}".format(hex(frame_refclks)))
        log.debug("INTEGRATION BYTES: {}".format(hex(irb[0]), hex(irb[1]), hex(irb[2]), hex(irb[3])))
        log.debug("FRAME BYTES: {}".format(hex(frb[0]), hex(frb[1]), hex(frb[2]), hex(frb[3])))

        # write the frame time register
        self.spi.writebytes([ft0 | reg_wr, frb[0]])
        self.spi.writebytes([ft1 | reg_wr, frb[1]])
        self.spi.writebytes([ft2 | reg_wr, frb[2]])
        self.spi.writebytes([ft3 | reg_wr, frb[3]])

        # write the integration time register
        self.spi.writebytes([it0 | reg_wr, irb[0]])
        self.spi.writebytes([it1 | reg_wr, irb[1]])
        self.spi.writebytes([it2 | reg_wr, irb[2]])
        self.spi.writebytes([it3 | reg_wr, irb[3]])

        # wait a sec for it to apply
        time.sleep(0.25)

        # read back 
        self.spi.writebytes([ft0])
        read = self.spi.readbytes(1)
        log.debug("ft0: {}".format(hex(read[0]), read[0]))

        self.spi.writebytes([ft1])
        read = self.spi.readbytes(1)
        log.debug("ft1: {}".format(hex(read[0]), read[0]))

        self.spi.writebytes([ft2])
        read = self.spi.readbytes(1)
        log.debug("ft2: {}".format(hex(read[0]), read[0]))

        self.spi.writebytes([ft3])
        read = self.spi.readbytes(1)
        log.debug("ft3: {}".format(hex(read[0]), read[0]))

        # read back 
        self.spi.writebytes([it0])
        read = self.spi.readbytes(1)
        log.debug("it0: {}".format(hex(read[0]), read[0]))

        self.spi.writebytes([it1])
        read = self.spi.readbytes(1)
        log.debug("it1: {}".format(hex(read[0]), read[0]))

        self.spi.writebytes([it2])
        read = self.spi.readbytes(1)
        log.debug("it2: {}".format(hex(read[0]), read[0]))

        self.spi.writebytes([it3])
        read = self.spi.readbytes(1)
        log.debug("it3: {}".format(hex(read[0]), read[0]))


    def disable(self):
        log.info("Disabling PIRT1280...")
        gpio.setup(sensor_enable_gpio, gpio.OUT)
        gpio.set(sensor_enable_gpio, 0)


def main():
    p = pirt1280()

    # set debug logging
    if args.v:
        log.setLevel(logging.DEBUG)

    # act on args
    if args.enable:
        p.enable()
    elif args.disable:
        p.disable()
    elif args.integration:
        p.set_integration(args.integration)
    else:
        arg.print_help()
        
if __name__ == "__main__":
    main()

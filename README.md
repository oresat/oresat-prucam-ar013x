# oresat-linux-prucam
Kernel module and PRU firmware to interface to camera using a PRU.
This for the [AM335x] family of processors, e.g., BeagleBoard's [PocketBeagle].

**NOTE:** This was tested on Debian with kernel 4.14.71 and 4.19.79

## What is a PRU?
Programible Real-time Units. It a microcontroller that shares pins and other resource with the core processor allowing for custom interface to other hardware like cameras. See [TI PRU-ICSS webpage] for more details.

## Supported cameras
- [AR0130CS](https://www.onsemi.com/pub/Collateral/AR0130CS-D.PDF)

## How to use on Debian
### Configure device trees for PRU and camera
- Install the beagleboard device tree files and device tree compiler.
    - `$ sudo apt install bb-cape-overlays device-tree-compiler`
- clone `bb.org-overlays`
    - `$ git clone https://github.com/beagleboard/bb.org-overlays`
- Build and deploy custom device-tree overlay
    - `$ cp kernel_module/prudev-00A0.dts bb.org-overlays/src/arm/`
    - `$ cd bb.org-overlays/`
    - `$ make`
    - `$ sudo cp src/arm/prudev-00A0.dtbo /lib/firmware`
- Modify these lines in `/boot/uEnv.txt` to
    - `#enable_uboot_cape_universal=1` (comment this line out)
    - `dtb_overlay=/lib/firmware/prudev-00A0.dtbo`
    -  If using kernel 4.14 make sure this line is not commented out `uboot_overlay_pru=/lib/firmware/AM335X-PRU-RPROC-4-14-TI-00A0.dtbo` or if using kernel 4.19 make sure this line is not commented out `uboot_overlay_pru=/lib/firmware/AM335X-PRU-RPROC-4-19-TI-00A0.dtbo`.
- Reboot
    - `$ sudo reboot`
### Build and deploy PRU firmware
- Install the TI's PRU compiler.
    - `$ sudo apt install ti-pru-cgt-installer`
- Build and deploy pru firmware
    - `$ cd pru_code`
    - `$ sudo ./deploy.sh`
### Build and insert the kernel module
- Install the kernel headers
    - ``$ sudo apt-get install linux-headers-`uname -r` ``
- Build and insert kernel module
    - `$ cd kernel_module`
    - `$ make clean all`
    - `$ sudo make ins`
### Run test program
- Make and run test program
    - `$ make clean all`
    - `$ sudo ./test_camera`
    - This should generate a `capture_001.bmp` file in that directory. scp this
to your computer to see if it worked!


[TI PRU-ICSS webpage]:https://processors.wiki.ti.com/index.php/PRU-ICSS
[AM335x]:https://www.ti.com/processors/sitara-arm/am335x-cortex-a8/overview.html
[PocketBeagle]:https://beagleboard.org/pocket

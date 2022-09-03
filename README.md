# oresat-linux-prucam

[![License](https://img.shields.io/github/license/oresat/oresat-linux-prucam)](./LICENSE)
[![Issues](https://img.shields.io/github/issues/oresat/oresat-linux-prucam)](https://github.com/oresat/oresat-linux-prucam/issues)

Kernel module, device tree overlay, and PRU firmware to interface to camera
using the PRUs on the [AM335x].

_**NOTE:**_ This was tested on Debian with kernel 4.14 and 4.19.

## What is a PRU?

Programible Real-time Units. It a microcontroller that shares pins and other
resource with the core processor allowing for custom interface to other hardware
like cameras. See [TI PRU-ICSS webpage] for more details.

## Supported cameras

- [AR0130CS](https://www.onsemi.com/pub/Collateral/AR0130CS-D.PDF)
- [AR0134CS](https://www.onsemi.com/pub/Collateral/AR0134CS-D.PDF)

## Build and install prucam

### Device tree overlay

- Install dependencies: `$ sudo apt install device-tree-compiler`
- Compile dtbo: `$ make -C src/device_tree_overlay`
- Install dtbo: `$ sudo make -C src/device_tree_overlay install`
- Edit `/boot/uEnv.txt`
  - Change the `#dtb_overlay=<file8>.dtbo` line to `dtb_overlay=/lib/firmware/prudev-00A0.dtbo`
  - Make sure the correct pru rproc `uboot_overlay_pru=` line is not commented
  out (depends kernel version).
  - Make sure the `#enable_uboot_cap_universal=` line is commented out.
- Reboot system to apply device tree overlay: `$ sudo reboot`

### Kernel module

- Install the kernel headers: ``$ sudo apt-get install linux-headers-`uname -r` ``
- Build kernel module: `$ make -C src/kernel_module clean all`
- Insert kernel module: `$ sudo insmod src/kernel_module/prucam.ko`
- **Note:** To remove kernel module: `$ sudo rmmod prucam`

### PRU firmware

- Install dependencies: `$ sudo apt install ti-pru-cgt-v2.3 ti-pru-software-v6.0`
- Compile and install binaries for both PRUs: `sudo ./src/pru_code/deploy.sh`

## Test prucam

- `$ cd testing/camera-test-c`
- Compile: `$ make`
- Capture image:`$ sudo ./test_camera`
  - This will produce `capture_001.bmp`

## Debian package

`prucam-dkms` is the kernel module that provides the sysfs interfaces to the
pru and ar013x camera settings. This package will also install the compiled pru
firmware and the prucam dtbo. **NOTE** when install, it will auto load the module.

To build `prucam-dkms`:

- Install build dependencies: ``$ sudo apt install --no-install-recommends
  --no-install-suggestions debhelper fakeroot dkms linux-headers-`uname -r`
  device-tree-compiler ti-pru-cgt-v2.3 ti-pru-software-v6.0``
  - The `--no-install-*` flags are due to `dkms` installing `linux-headers`
    package for the wrong kernel version
- Build prucam Debian packages: `$ dpkg-buildpackage -us -uc`

[TI PRU-ICSS webpage]:https://processors.wiki.ti.com/index.php/PRU-ICSS
[AM335x]:https://www.ti.com/processors/sitara-arm/am335x-cortex-a8/overview.html

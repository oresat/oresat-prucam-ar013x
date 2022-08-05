# oresat-linux-prucam

[![License](https://img.shields.io/github/license/oresat/oresat-linux-prucam)](./LICENSE)
[![Issues](https://img.shields.io/github/issues/oresat/oresat-linux-prucam)](https://github.com/oresat/oresat-linux-prucam/issues)

Kernel module and PRU firmware to interface to camera using a PRU.
This for the [AM335x] family of processors, e.g., BeagleBoard's [PocketBeagle].

_**NOTE:**_ This was tested on Debian with kernel 4.14 and 4.19.

## What is a PRU?

Programible Real-time Units. It a microcontroller that shares pins and other
resource with the core processor allowing for custom interface to other hardware
like cameras. See [TI PRU-ICSS webpage] for more details.

## Supported cameras

- [AR0130CS](https://www.onsemi.com/pub/Collateral/AR0130CS-D.PDF)
- [AR0134CS](https://www.onsemi.com/pub/Collateral/AR0134CS-D.PDF)

## Debian packages

The repos can make two debian package for the star tracker baord: `prucam-dkms`
and `python3-prucam`.

- **python-dkms**: The kernel module that provides the sysfs interfaces to the
pru and ar013x camera settings. This package will also install the compiled pru
firmware and the prucam dtbo. **NOTE** when install, it will auto load the module.
- **python3-prucam**: A python library for interfaceing with the prucam kernel
module's sysfs files and the pru sysfs files. It proves control of the camera
(capture images and adjust camera settings) and power control options for the
pru. Also the library can load pru's firmware. See `python3-lib/` for more info.

### Build prucam Debian packages

- Install build dependencies: ``$ sudo apt install debhelper fakeroot dkms
linux-headers-`uname -r` device-tree-compiler git python3-setuptools python3-all
ti-pru-cgt-v2.3``
- Build prucam Debian packages: `$ dpkg-buildpackage -us -uc`

## Build prucam without packaging

### Build and insert the kernel module

- Install the kernel headers: ``$ sudo apt-get install linux-headers-`uname -r` ``
- Build kernel module: `$ make -C src/kernel_module clean all`
- Insert kernel module: `$ sudo insmod src/kernel_module/prucam.ko`
- **Note:** To remove kernel module: `$ sudo rmmod prucam`

### PRU firmware

- Install dependencies: `$ sudo apt install ti-pru-cgt-v2.3`
- Compile and install pru firmware `sudo ./src/pru_code/deploy.sh`

### Device tree overlay

- Install dependencies: `$ sudo apt install device-tree-compiler git device-tree-overlays`
- `$ git clone https://github.com/beagleboard/bb.org-overlays src/device_tree_overlay/bb.org-overlays`
- `$ cp src/device_tree_overlay/prudev-00A0.dts src/device_tree_overlay/bb.org-overlays/src/arm/`
- `$ make -C src/device_tree_overlay/bb.org-overlays`
- `$ sudo cp src/device_tree_overlay/bb.org-overlays/src/arm/prudev-00A0.dtbo /lib/firmware/`
- Edit `/boot/uEnv.txt`
  - Change the `#dtb_overlay=<file8>.dtbo` line to `dtb_overlay=/lib/firmware/prudev-00A0.dtbo`
  - Make sure the correct pru rproc `uboot_overlay_pru=` line is not commented
  out (depends kernel version).
  - Make sure `enable_uboot_cap_universal=` line is commented out.

[TI PRU-ICSS webpage]:https://processors.wiki.ti.com/index.php/PRU-ICSS
[AM335x]:https://www.ti.com/processors/sitara-arm/am335x-cortex-a8/overview.html
[PocketBeagle]:https://beagleboard.org/pocket
[DKMS]:https://wiki.archlinux.org/index.php/Dynamic_Kernel_Module_Support

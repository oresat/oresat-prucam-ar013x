# oresat-linux-prucam
Kernel module and PRU firmware to interface to camera using a PRU.
This for the [AM335x] family of processors, e.g., BeagleBoard's [PocketBeagle].

**NOTE:** This was tested on Debian with kernel 4.14.71 and 4.19.79

## What is a PRU?
Programible Real-time Units. It a microcontroller that shares pins and other resource with the core processor allowing for custom interface to other hardware like cameras. See [TI PRU-ICSS webpage] for more details.

## Supported cameras
- [AR0130CS](https://www.onsemi.com/pub/Collateral/AR0130CS-D.PDF)


## Debian packages
The repos can make two debian package for the star tracker baord: prucam-dkms and python3-prucam.
- python-dkms: The kernel module that provides the sysfs interfaces to the pru and ar013x camera settings. It is packaged with dkms. See `kernel_module/` for more info.
- python3-prucam: A python library for interfaceing with the prucam kernel module's sysfs files and the pru sysfs files. It proves control of the camera (capture images and adjust camera settings) and power control options for the pru. Also the library can load pru's firmware. See `python3-prucam/` for more info.


[TI PRU-ICSS webpage]:https://processors.wiki.ti.com/index.php/PRU-ICSS
[AM335x]:https://www.ti.com/processors/sitara-arm/am335x-cortex-a8/overview.html
[PocketBeagle]:https://beagleboard.org/pocket

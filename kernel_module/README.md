# prucam kernel module
The platform driver kernel module for interfacing to an ar013x camera using a PRU.

**Note:** it will require the pru firmware to be installed to work, see
`pru_code/` on how to install.
**Note:** it will require the device tree overlay installed, see 
`device_tree_overlay/`

## Build and install debian package (uses [DKMS])
- Install dependencies
    - ``$ sudo apt install linux-headers-`uname -r` dkms``
- Build and install rucam kernel module Debian package
    - `$ dpkg-buildpackage`
    - `$ sudo dpkg -i ../prucam-dkms_*.deb`

## Build and insert the kernel module for development (no packaging)
- Install the kernel headers
    - ``$ sudo apt-get install linux-headers-`uname -r` ``
- Build and insert kernel module
    - `$ make -C src/ clean all`
    - `$ sudo insmod src/prucam.ko`

[DKMS]:https://wiki.archlinux.org/index.php/Dynamic_Kernel_Module_Support

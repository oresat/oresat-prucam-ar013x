# prucam kernel module
The platform driver kernel module for interfacing to an ar013x camera using a PRU.

**Note:** This will require the pru firmware to be installed to work, see
`pru_code/` on how to install.

**Note:** This will require the device tree overlay installed, see 
`device_tree_overlay/`

## Build and install debian package (uses [DKMS])
- Install dependencies: ``$ sudo apt install linux-headers-`uname -r` dkms``
- Build prucam-dkms Debian package: `$ dpkg-buildpackage`
- Install prucam-dkms Debian package: `$ sudo dpkg -i ../prucam-dkms_*.deb`

## Build and insert the kernel module for development (no packaging)
- Install the kernel headers: ``$ sudo apt-get install linux-headers-`uname -r` ``
- Build kernel module: `$ make -C src/ clean all`
- Insert kernel module: `$ sudo insmod src/prucam.ko`
- Remove kernel module: `$ sudo rmmod prucam`

[DKMS]:https://wiki.archlinux.org/index.php/Dynamic_Kernel_Module_Support

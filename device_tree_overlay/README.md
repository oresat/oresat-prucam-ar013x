# Device Tree Overlay

## Configure device trees for PRU and camera
- Install device tree compiler.
    - `$ sudo apt install device-tree-compiler`
- Build and deploy custom device-tree overlay
    - `$ make clean all`
    - `$ sudo make deploy`
- Modify these lines in `/boot/uEnv.txt` to
    - `#enable_uboot_cape_universal=1` (comment this line out)
    - `dtb_overlay=/lib/firmware/prudev-00A0.dtbo`
    -  If using kernel 4.14 make sure this line is not commented out `uboot_overlay_pru=/lib/firmware/AM335X-PRU-RPROC-4-14-TI-00A0.dtbo` or if using kernel 4.19 make sure this line is not commented out `uboot_overlay_pru=/lib/firmware/AM335X-PRU-RPROC-4-19-TI-00A0.dtbo`.
- Reboot
    - `$ sudo reboot`

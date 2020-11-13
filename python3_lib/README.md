# python3-prucam
A quick python library for interfacing the the prucam kernel module.

## dependencies
- `python3-opencv prucam-dkms`

## Build debian package
- Install dependencies: `$ sudo apt install python3-stdeb`
- Make deb package: `$ python3 setup.py --command-packages=stdeb.command bdist_deb`

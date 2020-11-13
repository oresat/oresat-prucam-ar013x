# python3-prucam
A quick python library for interfacing the the prucam kernel module.

## dependencies
- `python3-pillow` aka `python3-pil`

## unittest
- `$ PYTHONPATH=".:./" pytest`

## build debian package
- `$ python3 setup.py --command-packages=stdeb.command bdist_deb`

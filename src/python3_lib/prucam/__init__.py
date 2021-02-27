"""A interface for controlling the pru and camera."""

from .camera import Camera, CONTEXT_A, CONTEXT_B
from .pru import PRUCam

MAJOR = 0
MINOR = 3
PATCH = 0

APP_NAME = "prucam"
APP_DESCRIPTION = "A python library for interfacing the prucam kernel module"
APP_VERSION = "{}.{}.{}".format(MAJOR, MINOR, PATCH)
APP_AUTHOR = "Ryan Medick"
APP_EMAIL = "rmedick@pdx.edu"
APP_URL = "https://github.com/oresat/oresat-linux-updater"
APP_LICENSE = "GPL-3.0"

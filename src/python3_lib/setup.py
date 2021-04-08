""" OreSat Linux PRUcam library Setup.py """

from setuptools import setup
import prucam

with open("README.md", "r") as f:
    long_description = f.read()

setup(
    name=prucam.APP_NAME,
    version=prucam.APP_VERSION,
    author=prucam.APP_AUTHOR,
    license=prucam.APP_LICENSE,
    description=prucam.APP_DESCRIPTION,
    long_description=long_description,
    author_email="rmedick@pdx.edu",
    maintainer="Ryan Medick",
    maintainer_email="rmedick@pdx.edu",
    url="https://github.com/oresat/oresat-linux-prucam",
    packages=['prucam'],
    install_requires=[
        "opencv-python",
    ],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
    ],
)

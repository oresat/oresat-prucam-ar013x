""" OreSat Linux PRUcam library Setup.py """

from setuptools import setup

with open("README.md", "r") as f:
    long_description = f.read()

setup(
    name="prucam",
    version="0.3.2",
    author="PSAS",
    license="GPL-3.0",
    description="A python library for interfacing the prucam kernel module",
    long_description=long_description,
    maintainer="PSAS",
    maintainer_email="oresat@pdx.edu",
    url="https://github.com/oresat/oresat-linux-prucam",
    packages=['prucam'],
    package_dir={'prucam': 'src/python3_lib/prucam'},
    install_requires=[
        "opencv-python",
    ],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
    ],
)

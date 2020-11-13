""" Setup.py """

from setuptools import setup

with open('README.md') as f:
    long_description = f.read()

setup(
    name="prucam",
    version="0.1.0",
    license="GPLv3",
    description="A quick library to interface with prucam kernel module.",
    long_description=long_description,
    author="Ryan Medick",
    author_email="rmedick@pdx.edu",
    maintainer="Ryan Medick",
    maintainer_email="rmedick@pdx.edu",
    url="https://github.com/oresat/oresat-linux-prucam",
    packages=['prucam'],
    install_requires=[
        "opencv",
    ],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
    ],
)

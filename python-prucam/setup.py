""" Setup.py """

import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="prucam",
    version="0.0.1",
    author="Ryan Medick",
    author_email="rmedick@pdx.edu",
    description="A quick pythonic library for prucam.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/oresat/oresat-linux-prucam",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GPLv3 License",
    ],
    python_requires='>=3.7',
)

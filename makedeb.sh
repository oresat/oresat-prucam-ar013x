#!/bin/bash

PACKAGE=prucam-ar013x
VERSION=$(git describe --tags --abbrev=0)
VERSION="${VERSION:1}" # remove leading 'v'

dpkg-buildpackage -us -uc

mv ../$PACKAGE*.deb .
rm -f ../$PACKAGE*

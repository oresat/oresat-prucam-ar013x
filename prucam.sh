#!/bin/bash -e

INSTALL_PATH=`ls -d /usr/src/prucam*`
DKMS_MODULE=`basename $INSTALL_PATH`
DKMS_MODULE=`echo "$DKMS_MODULE" | sed -r 's/-/\//g'`
BUILD_PATH=/var/lib/dkms/${DKMS_MODULE}/`uname -r`/armv7l/module

if [ $# = 1 ]; then
  sleep $1
fi

dkms build ${DKMS_MODULE}
insmod ${BUILD_PATH}/prucam.ko.xz

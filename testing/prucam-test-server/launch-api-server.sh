#!/bin/bash
set -x
set -e

# this is a VERY brittle script to setup the prucam at boot. Have fun!

if [ $(basename $(pwd)) != "oresat-linux-prucam" ]
then
  echo ERROR: must be run from \'oresat-linux-prucam\' directory
  exit 1
fi

if [ $UID -ne 0 ]
then
  echo ERROR: must be run as root
  exit 1
fi

# wait for PRU sysfs entry to appear. This service needs that driver,
# but can start before it is initialized
FILE=/sys/class/remoteproc/remoteproc1/state
while true
do
  # test if the remoteproc state file exists
  if test -f "$FILE"
  then
    # exit when we find it
    echo found remoteproc sysfs file
    sleep 1s
    break
  fi

  echo waiting for remoteproc to init...
  sleep 2s
  continue
done

# setup PRUs
cd src/pru_code
./deploy.sh || true

# set up kernel driver. 
cd ../kernel_module
rmmod prucam -f || true # make sure it is removed first
insmod prucam.ko
cd ../..

# finally, launch the API server
./testing/prucam-test-server/image-api-server.py


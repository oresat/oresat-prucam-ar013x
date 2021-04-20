#! /bin/bash

export PRU_CGT=/usr/share/ti/cgt-pru
# The script builds, uploads, and starts PRU image capture firmware 

echo "-Building project"
	make clean
	make || exit 1

echo "-Placing the firmware"
	sudo cp gen/prucam_pru0_fw.out /lib/firmware/am335x-pru0-fw
	sudo cp gen/prucam_pru1_fw.out /lib/firmware/am335x-pru1-fw

echo "-Rebooting"
echo "Rebooting pru-core 1"
sudo su -c 'echo "stop" > /sys/class/remoteproc/remoteproc2/state'
sudo su -c 'echo "start" > /sys/class/remoteproc/remoteproc2/state'
sleep 0.1s
echo "Rebooting pru-core 0"
sudo su -c 'echo "stop" > /sys/class/remoteproc/remoteproc1/state'
sudo su -c 'echo "start" > /sys/class/remoteproc/remoteproc1/state'


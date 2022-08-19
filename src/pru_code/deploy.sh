#! /bin/bash

export PRU_CGT=/usr/share/ti/cgt-pru
# The script builds, uploads, and starts PRU image capture firmware 

echo "-Building project"
	make clean
	make || exit 1

echo "-Placing the firmware"
	sudo cp gen/prucam_pru0_fw.out /lib/firmware/
	sudo cp gen/prucam_pru1_fw.out /lib/firmware/

echo "Setting PRU Firmware Name"
sudo su -c 'echo "prucam_pru0_fw.out" > /sys/class/remoteproc/remoteproc1/firmware'
sudo su -c 'echo "prucam_pru1_fw.out" > /sys/class/remoteproc/remoteproc2/firmware'
sleep 0.1s

echo "-Rebooting"
echo "Rebooting pru-core 1"
sudo su -c 'echo "stop" > /sys/class/remoteproc/remoteproc2/state'
sudo su -c 'echo "start" > /sys/class/remoteproc/remoteproc2/state'
sleep 0.1s

echo "Rebooting pru-core 0"
sudo su -c 'echo "stop" > /sys/class/remoteproc/remoteproc1/state'
sudo su -c 'echo "start" > /sys/class/remoteproc/remoteproc1/state'


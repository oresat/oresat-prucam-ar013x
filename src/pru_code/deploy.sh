#! /bin/bash

export PRU_CGT=/usr/share/ti/cgt-pru
# The script builds, uploads, and starts PRU image capture firmware 

# camera parallel bus inputs
sudo config-pin P1_36 pruin # PRU0_0 -> D0
sudo config-pin P1_33 pruin # PRU0_1 -> D1
sudo config-pin P2_32 pruin # PRU0_2 -> D2
sudo config-pin P2_30 pruin # PRU0_3 -> D3
sudo config-pin P1_31 pruin # PRU0_4 -> D4
sudo config-pin P2_34 pruin # PRU0_5 -> D5
sudo config-pin P2_28 pruin # PRU0_6 -> D6
sudo config-pin P1_20 pruin # PRU0_1 -> CLK

# debug outputs
sudo config-pin P1_29 pruout # PRU0_7 
sudo config-pin P2_24 pruout # PRU0_14 out
sudo config-pin P2_33 pruout # PRU0_15


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


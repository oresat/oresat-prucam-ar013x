#! /bin/bash

export PRU_CGT=/usr/share/ti/cgt-pru
# The script builds, uploads, and starts PRU image capture firmware 

PRU0_0=P1_36 #D0
PRU0_1=P1_33 #D1
PRU0_2=P2_32 #D2
PRU0_3=P2_30 #D3
PRU0_4=P1_31 #D4
PRU0_5=P2_34 #D5
PRU0_6=P2_28 #D6
PRU0_7=P1_29 #D7
PRU0_14=P2_22 #VSYNC
PRU0_15=P2_18 #HSYNC
PRU0_16=P1_20 #PIXCLK
PRU0_15O=P2_33 #OUT

echo "-Building project"
	make clean
	make || exit 1

echo "-Placing the firmware"
	sudo cp gen/prucam_pru0_fw.out /lib/firmware/am335x-pru0-fw
	sudo cp gen/prucam_pru1_fw.out /lib/firmware/am335x-pru1-fw

echo "-Configuring pinmux"
	KERNEL_VERSION=`uname -r`

	if [[ $KERNEL_VERSION =~ "4.14." ]]
	then
		config-pin -a $PRU0_0 pruin
		config-pin -q $PRU0_0
		config-pin -a $PRU0_1 pruin
		config-pin -q $PRU0_1
		config-pin -a $PRU0_2 pruin
		config-pin -q $PRU0_2
		config-pin -a $PRU0_3 pruin
		config-pin -q $PRU0_3
		config-pin -a $PRU0_4 pruin
		config-pin -q $PRU0_4
		config-pin -a $PRU0_5 pruin
		config-pin -q $PRU0_5
		config-pin -a $PRU0_6 pruin
		config-pin -q $PRU0_6
		config-pin -a $PRU0_7 pruin
		config-pin -q $PRU0_7
		config-pin -a $PRU0_14 pruin
		config-pin -q $PRU0_14
		config-pin -a $PRU0_15 pruin
		config-pin -q $PRU0_15
		config-pin -a $PRU0_16 pruin
		config-pin -q $PRU0_16
		config-pin -a $PRU0_15O pruout
		config-pin -q $PRU0_15O
	elif [[ $KERNEL_VERSION =~ "4.19." ]]
	then
		config-pin $PRU0_0 pruin
		config-pin $PRU0_1 pruin
		config-pin $PRU0_2 pruin
		config-pin $PRU0_3 pruin
		config-pin $PRU0_4 pruin
		config-pin $PRU0_5 pruin
		config-pin $PRU0_6 pruin
		config-pin $PRU0_7 pruin
		config-pin $PRU0_14 pruin
		config-pin $PRU0_15 pruin
		config-pin $PRU0_16 pruin
		config-pin $PRU0_15O pruout
	else
		echo 'ERROR: Unkown kernel.'
		exit 1
	fi

	#Setup Inputs
	config-pin $PRU0_0 pruin
	config-pin -q $PRU0_0
	config-pin $PRU0_1 pruin
	config-pin -q $PRU0_1
	config-pin $PRU0_2 pruin
	config-pin -q $PRU0_2
	config-pin $PRU0_3 pruin
	config-pin -q $PRU0_3
	config-pin $PRU0_4 pruin
	config-pin -q $PRU0_4
	config-pin $PRU0_5 pruin
	config-pin -q $PRU0_5
	config-pin $PRU0_6 pruin
	config-pin -q $PRU0_6
	config-pin $PRU0_7 pruin
	config-pin -q $PRU0_7
	config-pin $PRU0_14 pruin
	config-pin -q $PRU0_14
	config-pin $PRU0_15 pruin
	config-pin -q $PRU0_15
	config-pin $PRU0_16 pruin
	config-pin -q $PRU0_16
	config-pin $PRU0_15O pruout
	config-pin -q $PRU0_15O

echo "-Rebooting"
echo "Rebooting pru-core 1"
sudo su -c 'echo "stop" > /sys/class/remoteproc/remoteproc2/state'
sudo su -c 'echo "start" > /sys/class/remoteproc/remoteproc2/state'
sleep 0.1s
echo "Rebooting pru-core 0"
sudo su -c 'echo "stop" > /sys/class/remoteproc/remoteproc1/state'
sudo su -c 'echo "start" > /sys/class/remoteproc/remoteproc1/state'


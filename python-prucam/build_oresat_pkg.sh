#! /bin/bash

export PRU_CGT=/usr/share/ti/cgt-pru

# compile pru firmware
cd ../pru_code/
make clean all
cd -

# add pru firmware to src
mkdir src/prucam/fw
cp ../pru_code/am335x-pru0-fw src/prucam/fw/

# make OreSat debian package
python3 setup.py --command-packages=stdeb.command bdist_deb

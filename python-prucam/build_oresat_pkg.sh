#! /bin/bash
export PRU_CGT=/usr/share/ti/cgt-pru

# compile pru firmware
cd ../pru_code/
make clean all
cd -

# add pru firmware to src
if [[ ! -d prucam/fw ]]; then
    mkdir prucam/fw
fi
cp ../pru_code/gen/pru_code.out prucam/fw/

# make OreSat debian package
python3 setup.py --command-packages=stdeb.command bdist_deb

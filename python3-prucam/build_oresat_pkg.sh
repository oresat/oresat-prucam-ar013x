#! /bin/bash
export PRU_CGT=/usr/share/ti/cgt-pru

rm -rf deb_dist dist prucam-*.tar.gz *.egg-info LICENSE

# compile pru firmware
cd ../pru_code/
make clean all
cd -

# add pru firmware to src
if [[ ! -d src/prucam/fw ]]; then
    mkdir src/prucam/fw
fi
cp ../pru_code/gen/pru_code.out src/prucam/fw/

# make OreSat debian package
python3 setup.py --command-packages=stdeb.command bdist_deb

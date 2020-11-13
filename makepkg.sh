#!/bin/bash

##############################################################################
# device tree

DTO_DIR=src/device_tree_overlay
BB_DTO_DIR=${DTO_DIR}/bb.org-overlays

if [ ! -d ${BB_DTO_DIR} ]; then
    git clone https://github.com/beagleboard/bb.org-overlays ${BB_DTO_DIR}
fi

cp ${DTO_DIR}/prudev-00A0.dts ${BB_DTO_DIR}/src/arm/

make -C ${BB_DTO_DIR}

cp ${BB_DTO_DIR}/src/arm/prudev-00A0.dtbo ${DTO_DIR}

##############################################################################
# pru fw

export PRU_CGT=/usr/share/ti/cgt-pru

# compile pru firmware
make -C src/pru_code clean all

##############################################################################
# build prucam-ar013x-dkms

dpkg-buildpackage -us -uc

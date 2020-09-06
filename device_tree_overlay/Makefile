dto=prudev-00A0

all: module

overlay: $(dto).dtbo
	dtc -O dtb -o $(dto).dtbo -b 0 -@ $(dto).dts

deploy:
	cp -v $(dto).dtbo /lib/firmware

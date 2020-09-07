dto=prudev-00A0

all: overlay

overlay: $(dto).dtbo
	dtc -O dtb -o $(dto).dtbo -b 0 -@ $(dto).dts

deploy:
	cp $(dto).dtbo /lib/firmware

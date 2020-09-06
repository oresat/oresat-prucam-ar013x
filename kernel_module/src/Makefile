obj-m += prucam.o
prucam-objs += cam_gpio.o cam_i2c.o
mod_name=prucam.ko
ccflags-y := -std=gnu99 -Wno-declaration-after-statement #this disables the C90 warnings

all: module

module:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules $(ccflag-y)

clean:
	@make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean > /dev/null



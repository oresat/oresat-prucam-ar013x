obj-m += prucam_ar013x.o
prucam_ar013x-objs := prucam_ar013x_main.o cam_gpio.o cam_i2c.o
mod_name=prucam-ar013x.ko
ccflags-y := -std=gnu99 -Wno-declaration-after-statement #this disables the C90 warnings

all: module

module:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules $(ccflag-y)

clean:
	@make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean > /dev/null



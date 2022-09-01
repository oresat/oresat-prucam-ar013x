obj-m += prucam.o
prucam-objs := prucam_main.o cam_gpio.o cam_i2c.o ar013x_sysfs.o
mod_name=prucam.ko
ccflags-y := -std=gnu99 -Wno-declaration-after-statement #this disables the C90 warnings

KDIR ?= /lib/modules/`uname -r`/build

all: module

module:
	make -C $(KDIR) M=$(PWD) modules $(ccflag-y)

clean:
	@make -C $(KDIR) M=$(PWD) clean



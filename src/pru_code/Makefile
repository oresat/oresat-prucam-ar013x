ifndef PRU_CGT # Can be set by a environment variable
PRU_CGT=/usr/share/ti/cgt-pru
endif

MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))
PROJ_NAME=$(CURRENT_DIR)
LINKER_COMMAND_FILE=./AM335x_PRU.cmd
TI_PRU_SW_PKG_PATH=/usr/lib/ti/pru-software-support-package-v6.0
LIBS=--library=$(TI_PRU_SW_PKG_PATH)/lib/rpmsg_lib.lib
INCLUDE=--include_path=$(TI_PRU_SW_PKG_PATH)/include --include_path=$(TI_PRU_SW_PKG_PATH)/include/am335x
STACK_SIZE=0x100
HEAP_SIZE=0x100
GEN_DIR=gen

# Common compiler and linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
# Additionally we define SPEED=1 configure the assembly capture loop for
# 0-50MHz pixel clock
CFLAGS=-v3 -O0 --display_error_number --endian=little --hardware_mac=on --obj_directory=$(GEN_DIR) --pp_directory=$(GEN_DIR) -ppd -ppa --asm_define=SPEED=1 --symdebug:none
# Linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
LFLAGS=--reread_libs --warn_sections --stack_size=$(STACK_SIZE) --heap_size=$(HEAP_SIZE) 
MAP=$(GEN_DIR)/$(PROJ_NAME).map

# NOTE: source files must match pru*_fw.c pru*_asm.s
PRU0_OUT=$(GEN_DIR)/prucam_pru0_fw.out
PRU1_OUT=$(GEN_DIR)/prucam_pru1_fw.out
PRU0_OBJECTS=$(GEN_DIR)/pru0_fw.object $(GEN_DIR)/pru0_asm.object
PRU1_OBJECTS=$(GEN_DIR)/pru1_fw.object $(GEN_DIR)/pru1_asm.object

.PHONY: all clean install

all: $(PRU0_OUT) $(PRU1_OUT)

$(PRU0_OUT): $(PRU0_OBJECTS) $(LINKER_COMMAND_FILE)
	@echo 'Building target: $@'
	@echo 'linking $<'
	@/usr/bin/clpru $(CFLAGS) -z -i$(PRU_CGT)/lib -i$(PRU_CGT)/include $(LFLAGS) -o $@ $(PRU0_OBJECTS) -m$(MAP) $(LINKER_COMMAND_FILE) --library=libc.a $(LIBS)

$(PRU1_OUT): $(PRU1_OBJECTS) $(LINKER_COMMAND_FILE)
	@echo 'Building target: $@'
	@echo 'linking $<'
	@/usr/bin/clpru $(CFLAGS) -z -i$(PRU_CGT)/lib -i$(PRU_CGT)/include $(LFLAGS) -o $@ $(PRU1_OBJECTS) -m$(MAP) $(LINKER_COMMAND_FILE) --library=libc.a $(LIBS)

$(GEN_DIR)/%.object: %.c
	@mkdir -p $(GEN_DIR)
	@echo 'compiling C file $<'
	@/usr/bin/clpru --include_path=$(PRU_CGT)/include $(INCLUDE) $(CFLAGS) -fe $@ $<

$(GEN_DIR)/%.object: %.s
	@mkdir -p $(GEN_DIR)
	@echo 'compiling assembly file $<'
	@/usr/bin/clpru --include_path=$(PRU_CGT)/include $(INCLUDE) $(CFLAGS) -fe $@ $<

clean:
	@rm -rf $(GEN_DIR)

install:
	cp $(GEN_DIR)/prucam_pru*_fw.out /lib/firmware/

# Includes the dependencies that the compiler creates (-ppd and -ppa flags)
-include $(PRU0_OBJECTS:%.object=%.pp)

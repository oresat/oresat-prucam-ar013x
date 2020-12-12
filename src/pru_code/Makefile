# PRU_CGT environment variable must point to the TI PRU compiler directory. E.g.:
#(Linux) export PRU_CGT=/home/jason/ti/ccs_v6_1_0/ccsv6/tools/compiler/ti-cgt-pru_2.1.0
#(Windows) set PRU_CGT=C:/TI/ccs_v6_0_1/ccsv6/tools/compiler/ti-cgt-pru_2.1.0
ifndef PRU_CGT
define ERROR_BODY

************************************************************
PRU_CGT environment variable is not set. Examples given:
(Linux) export PRU_CGT=/home/jason/ti/ccs_v6_1_0/ccsv6/tools/compiler/ti-cgt-pru_2.1.0
(Windows) set PRU_CGT=C:/TI/ccs_v6_0_1/ccsv6/tools/compiler/ti-cgt-pru_2.1.0
************************************************************

endef
$(error $(ERROR_BODY))
endif

MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))
PROJ_NAME=$(CURRENT_DIR)
LINKER_COMMAND_FILE=./AM335x_PRU.cmd
LIBS=--library=lib/rpmsg_lib.lib #Vérifier la position de la library rpmsg_lib.lib
INCLUDE=--include_path=include --include_path=include/am335x
STACK_SIZE=0x100
HEAP_SIZE=0x100
GEN_DIR=gen

#Common compiler and linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
CFLAGS=-v3 -O2 --display_error_number --endian=little --hardware_mac=on --obj_directory=$(GEN_DIR) --pp_directory=$(GEN_DIR) -ppd -ppa # --keep_asm
#Linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
LFLAGS=--reread_libs --warn_sections --stack_size=$(STACK_SIZE) --heap_size=$(HEAP_SIZE) 
MAP=$(GEN_DIR)/$(PROJ_NAME).map

# NOTE: source files must match pru*_fw.c
PRU0_OUT=$(GEN_DIR)/pru0_fw.out
PRU1_OUT=$(GEN_DIR)/pru1_fw.out

all: $(PRU0_OUT) $(PRU1_OUT)

$(GEN_DIR)/%.out: %.object $(LINKER_COMMAND_FILE)
	@echo 'Building target: $@'
	@echo 'linking $<'
	/usr/bin/clpru $(CFLAGS) -z -i$(PRU_CGT)/lib -i$(PRU_CGT)/include $(LFLAGS) -o $@ $< -m$(MAP) $(LINKER_COMMAND_FILE) --library=libc.a $(LIBS)

%.object: %.c
	@mkdir -p $(GEN_DIR)
	@echo 'compiling $<'
	/usr/bin/clpru --include_path=$(PRU_CGT)/include $(INCLUDE) $(CFLAGS) -fe $@ $<

.PHONY: all clean

# Remove the $(GEN_DIR) directory
clean:
	@echo 'Cleaning project: $(PROJ_NAME)'
	@echo 'Removing files in the "$(GEN_DIR)" directory'
	rm -rf $(GEN_DIR)
	rm -f *.asm
	@echo 'Finished cleaning project: $(PROJ_NAME)'

# Includes the dependencies that the compiler creates (-ppd and -ppa flags)
-include $(PRU0_OBJECTS:%.object=%.pp)

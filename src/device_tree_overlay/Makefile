NAME=prucam-00A0
DTC_FLAGS= -Wno-unit_address_vs_reg -Wno-chosen_node_is_root -Wno-alias_paths

all:
	cpp -nostdinc -I include -undef -x assembler-with-cpp $(NAME).dtso $(NAME).dtso.tmp
	dtc -O dtb -o $(NAME).dtbo -b 0 -@ $(DTC_FLAGS) $(NAME).dtso.tmp

clean:
	rm -rf *.dtbo *.tmp

install:
	cp $(NAME).dtbo /lib/firmware/

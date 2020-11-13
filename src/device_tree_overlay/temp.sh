md_dtc = 
$(CPP) $(dtc_cpp_flags) -x assembler-with-cpp -o $(dtc-tmp) $< 
$(DTC) -O dtb -o $@ -b 0 -@ -i $(src) $(DTC_FLAGS) -d $(depfile).dtc.tmp $(dtc-tmp)
cat $(depfile).pre.tmp $(depfile).dtc.tmp > $(depfile)

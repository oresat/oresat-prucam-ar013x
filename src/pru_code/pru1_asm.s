;* Import all symbols from the C file
	.cdecls "pru1_fw.c"

;* C declaration:
;* void run(struct capture_context *ctx, u32 trigger_flags)
	.clink
	.global run_asm1
run_asm1:
  ; r14 contains the current address of the image buffer
  ; r15 contains the address of the current line buffer
  

  jmp     r3.w2 ;jump to link register to return TODO is this all?



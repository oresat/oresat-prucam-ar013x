;* Import all symbols from the C file
	.cdecls "pru0_fw.c"

;* C declaration:
;* void run(struct capture_context *ctx, u32 trigger_flags)
	.clink
	.global run_asm0
run_asm0:
  wbc r31, 2
  add r27, r27, 0



;* Import all symbols from the C file
	.cdecls "pru1_fw.c"

; C declaration:
; void do_transfer(uint8_t* addr);
; Argument 'addr' contains the base address of the image buffer
; and is passed in R14
	.clink
	.global do_transfer
do_transfer:
  ; signal the other PRU to start the capture
  ldi r31, SYS_EVT_16_TRIGGER

  ; r18 contains the number of lines left in the image. 
  ldi r18, ROWS

LINE_RESTART:
  ; r17 contains the number of 32 byte chunks left in in the image line. 
  ldi r17, LINE_CHUNKS

CHUNK_RESTART:
  ; wait for signal from other PRU to start frame capture
  wbs r31, PRU0_TO_PRU1_R31_BIT

  ; clear the system event interrupt in INTC SICR register. Here we use the
  ; constants tables to address the register for efficiency
  ldi r16, PRU0_TO_PRU1_EVENT
  sbco &r16, INTC_CO_TABLE_ENTRY, SICR_REG_OFFSET, 4

  ; read in 32 bytes of image data from r22-r29
  xin 10, &r22, 32

  ; store image data to buffer address in R14
  sbbo &r22, r14, 0, 32

  ; increment the image buffer pointer
  add r14, r14, CHUNK_SIZE

  ; decrement the chunk counter
  sub r17, r17, 1

  ; if we still have chunks left in the line, restart another chunk transfer
  qblt CHUNK_RESTART, r17, 0

  ; decrement the line counter
  sub r18, r18, 1

  ; if we still have lines left in the image, restart another line transfer
  qblt LINE_RESTART, r18, 0

  ; trigger interrupt 20 to tell kernel the transfer is complete
  ldi r31, SYS_EVT_20_TRIGGER
  ; TODO do I need to clear this interrupt? I haven't in the past...

  jmp     r3.w2 ; jump to link register to return 


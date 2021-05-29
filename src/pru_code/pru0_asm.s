;* Import all symbols from the C file
	.cdecls "pru0_fw.c"

;* C declaration:
;* void capture_frame_8b(void)
	.clink
	.global capture_frame_8b
capture_frame_8b:
  ; wait for signal from PRU1 to start frane capture
  wbs r31, PRU1_TO_PRU0_R31_BIT

  ; clear the system event interrupt in INTC SICR register. Here we use the
  ; constants tables to address the register for efficiency
  ldi r16, PRU1_TO_PRU0_EVENT
  sbco &r16, INTC_CO_TABLE_ENTRY, SICR_REG_OFFSET, 4

  ldi r30, 0x0000

  ; r16.w0 holds the number of lines left in the image(rows) and r16.w2 holds
  ; the number of pixels left in the image line(columns). Here we preload them 
  ; so they can be decremented later. Each use 2 bytes from R16 because rows 
  ; and columns are both <65536. A larger capture in the future might need
  ; standalone counters. We start one less that the number of rows because
  ; FRAME_START preceeds the first row
  ldi r16.w0, ROWS
  ldi r16.w2, COLS ; reload number of pixels in row ; TODO need to interleave this

FOREVER:
  wbs r31, CLK_BIT
  QBNE FOREVER, r31.b0, FS1_1
  nop
  
  wbs r31, CLK_BIT
  QBNE FOREVER, r31.b0, FS1_2

  wbs r31, CLK_BIT
  QBNE FOREVER, r31.b0, FS2_1
  nop

  wbs r31, CLK_BIT
  QBNE FOREVER, r31.b0, FS2_2
  ldi r30, DEBUG1
  
  ; since we dont have time to branch, read the first chunk here
  wbs r31, CLK_BIT
  and r22.b0, r31.b0, 0x7F
  qba FIRST_READ_LINE
 
  ; save the LS symbol in a register in big and little endian formats so
  ; we can use them to compare later
  ; TODO is it ok to use REG21? 
  ldi r21.w0, LS_LE
  ldi r21.w2, LS_BE

LINE_RESTART:
  
  ; wait for LS1 ; TODO I can replace this I think
  wbs r31, CLK_BIT
  and r22.b0, r31.b0, 0x7F
  qbne LINE_RESTART, r22.b0, LS_1

SEARCH:
  ; search for little endian
  wbs r31, CLK_BIT
  and r22.b1, r31.b0, 0x7F
  qbeq READ_LINE, r22.w0, r21.w0

  wbs r31, CLK_BIT
  and r22.b2, r31.b0, 0x7F
  qbeq READ_LINE, r22.w1, r21.w0

  wbs r31, CLK_BIT
  and r22.b3, r31.b0, 0x7F
  qbeq READ_LINE, r22.w2, r21.w0

  ; search for big endian
  wbs r31, CLK_BIT
  and r22.b2, r31.b0, 0x7F
  qbeq READ_LINE, r22.w2, r21.w2

  wbs r31, CLK_BIT
  and r22.b1, r31.b0, 0x7F
  qbeq READ_LINE, r22.w1, r21.w2

  wbs r31, CLK_BIT
  and r22.b0, r31.b0, 0x7F
  qbne SEARCH, r22.w0, r21.w2

READ_LINE:
; TODO MUST immediately read here without waiting for clock

  ; now we start the tranfer to r22-r29 for a total of 32 bytes. Each capture
  ; is 1 byte and consists of the timing routine, 1 cycle to read in the byte 
  ; from r31 i.e. pixel value, and 1 cycle for either a nop or a control 
  ; instruction like xin/xout transfer, triggering an interrupt, or loop 
  ; maintenance. The timing routine is defined at compile time and consists of
  ; different instructions based on the SPEED symbol
  
  mov r22.b0, r31.b0
  ldi r30, DEBUG2

FIRST_READ_LINE:

  wbs r31, CLK_BIT
  mov r22.b1, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r22.b2, r31.b0
  ldi r30, 0x0000 ; de-assert debug lines

  ;jmp FOREVER
; we start the loop a couple operations after the beginning as these first
; couple bytes are recorded at the end so their NOP can be interleaved with
; other instructions that must run at the end 
CHUNK_RESTART:

  wbs r31, CLK_BIT
  mov r22.b3, r31.b0
  nop

  ; save to reg 23
  wbs r31, CLK_BIT
  mov r23.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r23.b1, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r23.b2, r31.b0
  ldi r30, 0x0000 ; de-assert debug lines

  wbs r31, CLK_BIT
  mov r23.b3, r31.b0
  nop

  ; save to reg 24
  wbs r31, CLK_BIT
  mov r24.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r24.b1, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r24.b2, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r24.b3, r31.b0
  nop

  ; save to reg 25
  wbs r31, CLK_BIT
  mov r25.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r25.b1, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r25.b2, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r25.b3, r31.b0
  nop

  ; save to reg 26
  wbs r31, CLK_BIT
  mov r26.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r26.b1, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r26.b2, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r26.b3, r31.b0
  nop

  ; save to reg 27
  wbs r31, CLK_BIT
  mov r27.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r27.b1, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r27.b2, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r27.b3, r31.b0
  nop

  ; save to reg 28
  wbs r31, CLK_BIT
  mov r28.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r28.b1, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r28.b2, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r28.b3, r31.b0
  nop

  ; save to reg 29
  wbs r31, CLK_BIT
  mov r29.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  mov r29.b1, r31.b0
  nop


  wbs r31, CLK_BIT
  mov r29.b2, r31.b0
  ; decrement our pixel counter by the number of pixels we have copied
  sub r16.w2, r16.w2, CHUNK_SIZE

  wbs r31, CLK_BIT
  mov r29.b3, r31.b0
  ; swap registers r22-r29 i.e. 32 bytes to scratchpad from where the other PRU
  ; can copy them to RAM.
  xout SCRATCHPAD_BANK_0, &r22, CHUNK_SIZE 

  ; above we finished reading in the 32 bytes of the chunk and transfered it
  ; with the XOUT instruction. However, we still need to notify the other PRU
  ; core to save the bytes to DRAM and then branch to the beginning of another
  ; chunk transfer. Since we must read a pixel every 4 cycles, below we start
  ; reading the beginning of the chunk to r22 and interleave the remaining
  ; maintenance instructions and branch back to the beginning of the chunk
  ; transfer minus the couple r22 bytes we read below

  ; save to reg 22 byte 0
  wbs r31, CLK_BIT
  mov r22.b0, r31.b0
  ; signal the other PRU to start the transfer
  ldi r31, SYS_EVT_17_TRIGGER

  ; save to reg 22 byte 1
  wbs r31, CLK_BIT
  mov r22.b1, r31.b0
  nop

  ; save to reg 22 byte 2
  wbs r31, CLK_BIT
  mov r22.b2, r31.b0
  ; if we still have pixels left to read, branch back to CHUNK_RESTART
  qblt CHUNK_RESTART, r16.w2, 0

  ; reload number of pixels in row 
  ldi r16.w2, COLS 

  ; decrement the row counter and restart the line capture if there are lines
  ; left in the image. Since we finished the line, we no longer need to
  ; precisely time and interleave instructions because there is slack time
  ; between lines
  sub r16.w0, r16.w0, 1
  qblt LINE_RESTART, r16.w0, 0

  ldi r30, DEBUG3
  ldi r30, DEBUG3
  ldi r30, 0x0000 ; de-assert debug lines
  ldi r30, 0x0000 ; de-assert debug lines

  ; once we have read all the lines in the image, return to the caller
  jmp     r3.w2 ; jump to link register to return 


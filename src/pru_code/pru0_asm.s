;* Import all symbols from the C file
	.cdecls "pru0_fw.c"

;* C declaration:
;* void capture_frame_8b(void)
	.clink
	.global capture_frame_8b
capture_frame_8b:
  ; wait for signal from kernel to start frame capture
  wbs r31, KERNEL_TO_PRUS_R31_BIT

  ; clear the system event interrupt in INTC SICR register. Here we use the
  ; constants tables to address the register for efficiency
  ldi r16, KERNEL_TO_PRUS_EVENT
  sbco &r16, INTC_CO_TABLE_ENTRY, SICR_REG_OFFSET, 4

  ldi r30, 0x0000

  ; save the LS symbol in a register in big and little endian formats so
  ; we can use them to compare later
  ; TODO is it ok to use REG21? 
  ldi r21.w0, LS_LE
  ldi r21.w2, LS_BE


  ; r16.w0 holds the number of lines left in the image(rows) and r16.w2 holds
  ; the number of pixels left in the image line(columns). Here we preload them 
  ; so they can be decremented later. Each use 2 bytes from R16 because rows 
  ; and columns are both <65536. A larger capture in the future might need
  ; standalone counters. We start one less that the number of rows because
  ; FRAME_START preceeds the first row
  ldi r16.w0, ROWS
  ldi r16.w2, COLS ; reload number of pixels in row ; TODO need to interleave this

FIND_FS:
  wbs r31, CLK_BIT
  qbne FIND_FS, r31.b0, FS1_1
  ldi r30, 0x0000
  
  wbs r31, CLK_BIT
  qbne FIND_FS, r31.b0, FS1_2
  nop

  wbs r31, CLK_BIT
  qbne FIND_FS, r31.b0, FS2_1
  ldi r30, DEBUG1 ; set DEBUG1 even though we haven't quite found FS2 yet

  wbs r31, CLK_BIT
  qbne FIND_FS, r31.b0, FS2_2
  qba READ_LINE_NO_LINE_NUMBER
 
LINE_RESTART:
  
  ; read in the first byte to b0. Only run this on branch to LINE_RESTART 
  ; because SEARCH below will write b0 in the last routine 
  wbs r31, CLK_BIT
  and r22.b0, r31.b0, 0x7F
  nop

SEARCH:
  ; here we search for the LS 0x7F, 0x7D pattern. We continually read in bytes,
  ; checking after every read if the past 2 bytes match the pattern. In order
  ; to do this in a loop we first read in B0, B1, B2, and B3 of reg 22,
  ; checking B0-B1, B1-B2, and B2-B3, for the little-endian representation of
  ; LS. Then we read in B2, B1, and B0 and check B3-B2, B2-B1, and B1-B0 for
  ; the big-endian representation of LS. Then we repeat. This way, we can 
  ; constantly look for the symbol in the stream while maintaining a read every
  ; 3 cycles.

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
  
  ; look for big-endian LS in B1-B0, and if not found, repeat, otherwise fall
  ; through to start the line read
  wbs r31, CLK_BIT
  and r22.b0, r31.b0, 0x7F
  qbne SEARCH, r22.w0, r21.w2

READ_LINE:

  ; after the LS symbol, the line number is written out in 2 bytes(1 pixel).
  ; Here we skip those 2 bytes so they don't end up in the image. HOWEVER,
  ; removing below will encode the line number into the final image buf, which
  ; can be very useful in debugging, so keep that in mind.
  wbs r31, CLK_BIT
  nop
  nop
  wbs r31, CLK_BIT
  nop
  nop

; no line number is encoded in the first line after the FS symbols, so start here
READ_LINE_NO_LINE_NUMBER:

  ; now we start the tranfer to r22-r29 for a total of 32 bytes. Each capture
  ; is 1 byte and consists of the timing routine, 1 cycle to read in the byte 
  ; from r31 i.e. pixel value, and 1 cycle for either a nop or a control 
  ; instruction like xin/xout transfer, triggering an interrupt, or loop 
  ; maintenance. 
  ;
  ; Pixel values are 14 bits, 7 bits per capture, with the MSB coming first.
  ; However, it makes the user's job easier if we capture the data as little
  ; endian. So, the first byte of every pixel gets shifted left 7 bits and 
  ; stores into w0(bits 0-15) of the destination register. The second byte of 
  ; the pixel gets OR'd with the existing value of b0(bits 0-7) of the
  ; destination register. This way, the LSb of the first captured byte is the
  ; MSb of the LSB of final value. This same routine happens with the next 2
  ; captured bytes, but is put into the higher 2 bytes of the register
  ; TODO example??
  
  ; shift first byte of pixel 7 bits to the left and save it to word0 of
  ; register r22
  wbs r31, CLK_BIT
  lsl r22.w0, r31.b0, 7
  ldi r30, DEBUG2

  ; or the second byte of the pixel with the value of r22.b0, which should be
  ; all zeros except possibly bit 7, which could have been written above
  wbs r31, CLK_BIT
  or r22.b0, r22.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  lsl r22.w2, r31.b0, 7
  ldi r30, 0x0000 ; de-assert debug lines

; we start the loop a couple operations after the beginning as these first
; couple bytes are recorded at the end so their NOP can be interleaved with
; other instructions that must run at the end 
CHUNK_RESTART:

  wbs r31, CLK_BIT
  or r22.b2, r22.b2, r31.b0
  nop

  ; start pixel 2(out of 0) of chunk
  ; save to reg 23
  wbs r31, CLK_BIT
  lsl r23.w0, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r23.b0, r23.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  lsl r23.w2, r31.b0, 7
  ldi r30, 0x0000 ; de-assert debug lines

  wbs r31, CLK_BIT
  or r23.b2, r23.b2, r31.b0
  nop

  ; start pixel 4 of chunk
  ; save to reg 24
  wbs r31, CLK_BIT
  lsl r24.w0, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r24.b0, r24.b0, r31.b0
  nop

  ; start pixel 5 of chunk
  wbs r31, CLK_BIT
  lsl r24.w2, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r24.b2, r24.b2, r31.b0
  nop

  ; save to reg 25
  wbs r31, CLK_BIT
  lsl r25.w0, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r25.b0, r25.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  lsl r25.w2, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r25.b2, r25.b2, r31.b0
  nop

  ; save to reg 26
  wbs r31, CLK_BIT
  lsl r26.w0, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r26.b0, r26.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  lsl r26.w2, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r26.b2, r26.b2, r31.b0
  nop

  ; save to reg 27
  wbs r31, CLK_BIT
  lsl r27.w0, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r27.b0, r27.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  lsl r27.w2, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r27.b2, r27.b2, r31.b0
  nop

  ; save to reg 28
  wbs r31, CLK_BIT
  lsl r28.w0, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r28.b0, r28.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  lsl r28.w2, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r28.b2, r28.b2, r31.b0
  nop

  ; save to reg 29
  wbs r31, CLK_BIT
  lsl r29.w0, r31.b0, 7
  nop

  wbs r31, CLK_BIT
  or r29.b0, r29.b0, r31.b0
  nop

  ; save to reg 29
  wbs r31, CLK_BIT
  lsl r29.w2, r31.b0, 7
  ; decrement our pixel counter by the number of pixels we have copied
  sub r16.w2, r16.w2, CHUNK_SIZE

  wbs r31, CLK_BIT
  or r29.b2, r29.b2, r31.b0
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

  ; save first pixel of next chunk to reg 22 
  wbs r31, CLK_BIT
  lsl r22.w0, r31.b0, 7
  ; signal the other PRU to start the transfer
  ldi r31, SYS_EVT_17_TRIGGER

  wbs r31, CLK_BIT
  or r22.b0, r22.b0, r31.b0
  nop

  wbs r31, CLK_BIT
  lsl r22.w2, r31.b0, 7
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


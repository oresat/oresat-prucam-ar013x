;* Import all symbols from the C file
	.cdecls "pru0_fw.c"

  ; Assert that SPEED is defined 
  .if $defined(SPEED) 
    .if SPEED = 1
      .mmsg "Low Speed Set"
    .elseif SPEED = 2
      .mmsg "High Speed Set"
    .elseif SPEED = 3
      .mmsg "100MHz Speed Set"
    .else
      .emsg "SPEED incorrectly defined"
    .endif
  .else
    .emsg "SPEED not defined"
  .endif

; __timing_routine inserts the required instructions for the compile time 
; defined sample rate. It is based on the SPEED symbol, possible values are:
; - 1: capture loop is for 0-50MHz pixel clock.
; - 2: capture loop is for 33.33MHz-66.66MHz pixel clock.
; - 3: capture loop is for (exactly) 100MHz pixel clock. UNTESTED!
__timing_routine .macro
  ; SPEED == 3: capture loop is for 100MHz pixel clock. SPEED=3 inserts no
  ; additional instructions so the capture routine only executes the pixel
  ; capture and the NOP(or control instruction). These 2 instructions take
  ; 10ns, which creates 100MHz sample rate (1/10ns = 100MHz). This is only 
  ; suitable for a pixel clock of exactly 100MHz
  .if SPEED = 3

  ; SPEED == 2: capture loop is for 33.33MHz-66.66MHz pixel clock. This routine
  ; waits for the pixel clock to be high. This routine can capture a pixel
  ; clock maximum of 66.66MHz. This route can capture a minimum pixel clock of
  ; 33.33MHz because it does not wait for the clock to go low. The capture code
  ; will wait for the pixel clock to be high, capture the pixel value, and NOP
  ; (or control instruction). These 3 instructions take a minimum of 15ns 
  ; (5ns * 3), so a pixel clock <33.33MHz would have a high time of greater
  ; than 15ns, which would mean we could potentially re-execute the 
  ; 'WBS R31, CLK_BIT' during the same cycle, screwing up the capture.
  ; Therefore, SPEED=2 is only suitable for 33.33MHz-66.66MHz
  .elseif SPEED = 2
    wbs r31, CLK_BIT

  ; SPEED == 1: capture loop is for 0-50MHz pixel clock. This routine waits for
  ; the pixel clock to go low and then high. This routine can capture a pixel
  ; clock maximum of 50Mhz. The capture code will wait for the clock to go low
  ; and then high, capture the pixel data, and NOP(or control instruction like 
  ; xin/xout transfer, triggering an interrupt, or loop maintenance.). These 4 
  ; instructions take a minimum of 20ns (5ns * 4), so can sample at a maximum 
  ; rate of 50MHz (1/20ns)  
  .elseif SPEED =  1
    wbc r31, CLK_BIT
    wbs r31, CLK_BIT

  ; error if speed is not defined
  .else
    .emsg "SPEED NOT DEFINED"
  .endif
  .endm

;* C declaration:
;* void capture_frame_8b(void)
	.clink
	.global capture_frame_8b
capture_frame_8b:
  ; wait for signal from PRU1 to start frane capture
  ;wbs r31, PRU1_TO_PRU0_R31_BIT

  ; clear the system event interrupt in INTC SICR register. Here we use the
  ; constants tables to address the register for efficiency
  ;ldi r16, PRU1_TO_PRU0_EVENT
  ;sbco &r16, INTC_CO_TABLE_ENTRY, SICR_REG_OFFSET, 4

  ;ldi r30, 0xffff
  ;ldi r30, 0x0000

FOREVER:
  __timing_routine
  and r22.b0, r31.b0, 0x7F
  QBNE FOREVER, r22.b0, FS1_1
  
  __timing_routine
  and r22.b0, r31.b0, 0x7F
  QBNE FOREVER, r22.b0, FS1_2

  __timing_routine
  and r22.b0, r31.b0, 0x7F
  QBNE FOREVER, r22.b0, FS2_1
  __timing_routine
  and r22.b0, r31.b0, 0x7F
  QBNE FOREVER, r22.b0, FS2_2


  ldi r30, 0xffff
  ldi r30, 0x0000


  jmp FOREVER
  
  
  
  ; r16.w0 holds the number of lines left in the image(rows) and r16.w2 holds
  ; the number of pixels left in the image line(columns). Here we preload them 
  ; so they can be decremented later. Each use 2 bytes from R16 because rows 
  ; and columns are both <65536. A larger capture in the future might need
  ; standalone counters.
  ldi r16.w0, ROWS

; LINE_RESTART is where we branch back to on every subsequent line capture. It
; comes after VSYNC is asserted but before HSYNC is asserted
LINE_RESTART:
  ldi r16.w2, COLS ; reload number of pixels in row

  ; now we start the tranfer to r22-r29 for a total of 32 bytes. Each capture
  ; is 1 byte and consists of the timing routine, 1 cycle to read in the byte 
  ; from r31 i.e. pixel value, and 1 cycle for either a nop or a control 
  ; instruction like xin/xout transfer, triggering an interrupt, or loop 
  ; maintenance. The timing routine is defined at compile time and consists of
  ; different instructions based on the SPEED symbol
  
  ; save to reg 22
  __timing_routine
  mov r22.b0, r31.b0
  nop

  __timing_routine
  mov r22.b1, r31.b0
  nop

; we start the loop a couple operations after the beginning as these first
; couple bytes are recorded at the end so their NOP can be interleaved with
; other instructions that must run at the end 
CHUNK_RESTART:

  __timing_routine
  mov r22.b2, r31.b0
  nop

  __timing_routine
  mov r22.b3, r31.b0
  nop

  ; save to reg 23
  __timing_routine
  mov r23.b0, r31.b0
  nop

  __timing_routine
  mov r23.b1, r31.b0
  nop

  __timing_routine
  mov r23.b2, r31.b0
  nop

  __timing_routine
  mov r23.b3, r31.b0
  nop

  ; save to reg 24
  __timing_routine
  mov r24.b0, r31.b0
  nop

  __timing_routine
  mov r24.b1, r31.b0
  nop

  __timing_routine
  mov r24.b2, r31.b0
  nop

  __timing_routine
  mov r24.b3, r31.b0
  nop

  ; save to reg 25
  __timing_routine
  mov r25.b0, r31.b0
  nop

  __timing_routine
  mov r25.b1, r31.b0
  nop

  __timing_routine
  mov r25.b2, r31.b0
  nop

  __timing_routine
  mov r25.b3, r31.b0
  nop

  ; save to reg 26
  __timing_routine
  mov r26.b0, r31.b0
  nop

  __timing_routine
  mov r26.b1, r31.b0
  nop

  __timing_routine
  mov r26.b2, r31.b0
  nop

  __timing_routine
  mov r26.b3, r31.b0
  nop

  ; save to reg 27
  __timing_routine
  mov r27.b0, r31.b0
  nop

  __timing_routine
  mov r27.b1, r31.b0
  nop

  __timing_routine
  mov r27.b2, r31.b0
  nop

  __timing_routine
  mov r27.b3, r31.b0
  nop

  ; save to reg 28
  __timing_routine
  mov r28.b0, r31.b0
  nop

  __timing_routine
  mov r28.b1, r31.b0
  nop

  __timing_routine
  mov r28.b2, r31.b0
  nop

  __timing_routine
  mov r28.b3, r31.b0
  nop

  ; save to reg 29
  __timing_routine
  mov r29.b0, r31.b0
  nop

  __timing_routine
  mov r29.b1, r31.b0
  nop

  __timing_routine
  mov r29.b2, r31.b0
  ; decrement our pixel counter by the number of pixels we have copied
  sub r16.w2, r16.w2, CHUNK_SIZE

  __timing_routine
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
  __timing_routine
  mov r22.b0, r31.b0
  ; signal the other PRU to start the transfer
  ldi r31, SYS_EVT_17_TRIGGER

  ; save to reg 22 byte 1
  __timing_routine
  mov r22.b1, r31.b0
  ; if we still have pixels left to read, branch back to CHUNK_RESTART
  qblt CHUNK_RESTART, r16.w2, 0

  ; decrement the row counter and restart the line capture if there are lines
  ; left in the image. Since we finished the line, we no longer need to
  ; precisely time and interleave instructions because there is slack time
  ; between lines
  sub r16.w0, r16.w0, 1
  qblt LINE_RESTART, r16.w0, 0

  ; once we have read all the lines in the image, return to the caller
  jmp     r3.w2 ; jump to link register to return 


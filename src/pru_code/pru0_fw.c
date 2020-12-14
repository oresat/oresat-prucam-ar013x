#include "pru_fw.h"

// run_asm0 is declared in the *.s assembly file
extern void run_asm0(void);

void main(void)
{
  //run_asm0();
  //manually trigger r31:31 set bit 24 in PRU SRSR0 to trigger event 24
  //CT_INTC.SRSR0_bit.RAW_STS_31_0 = 0x1000000; 

  // set the shared var to the known address in shared RAM
  struct pru_shared_vars_t* shared = SHARED_VAR_ADDR;

  int line, chunk, i;

  while(1)
  {
    // without this delay somewhere in this section, blacks frames are often
    // returned! I don't know why this is.
    __delay_cycles(1);

    // wait for the signal from PRU1 and clear the event 16 after
    while((__R31 & 1U<<30) == 0);
    CT_INTC.SICR_bit.STS_CLR_IDX = 16; // TODO do I need to clear this?

    while(__R31 & VSYNC_MASK); // wait for VSYNC to go low
    while((__R31 & VSYNC_MASK) == 0); // wait for VSYNC to go high

    for(line = 0; line < ROWS; line++)
    {
      while((__R31 & HSYNC_MASK) == 0); // wait for HSYNC to go high

      for(chunk = 0; chunk < LINE_CHUNKS; chunk++) 
      {
        for(i = 0; i < CHUNK_SIZE; i++)
        {
          while(__R31 & CLK_MASK); // wait for R31[16] to go low 
          while((__R31 & CLK_MASK) == 0); // wait for R31[16] to go high

          // write LSB value from GPIO(R31) to buf
          shared->buf0[i] = __R31;
        }
        // trigger event 17 to tell PRU1 to start the transfer
        __R31 = SYS_EVT_17_TRIGGER;
      }


      /*
      // loop through every pixel in row
      for(col = 0; col < COLS; col++)
      {
      while(__R31 & CLK_MASK); // wait for R31[16] to go low 
      while((__R31 & CLK_MASK) == 0); // wait for R31[16] to go high

      // write LSB value from GPIO(R31) to buf
      shared->buf[col] = __R31;
      }

      // trigger event 17 to tell PRU1 to start the transfer
      __R31 = SYS_EVT_17_TRIGGER;
      */
    }
  }
}


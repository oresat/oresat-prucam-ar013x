#include "pru_fw.h"

extern void run_asm1(uint8_t* base, uint8_t* buf);

void main(void)
{
  // set the shared var to the known address in shared RAM
  struct pru_shared_vars_t* shared = SHARED_VAR_ADDR;

  int line, chunk;
  volatile uint8_t* base;

  //init PRU registers
  initPRU();

  while(1)
  {  
    __delay_cycles(1);

    // The kernel will write the address of the allocated memory location into
    // a known shared memory location. Here we read that address from that
    // location.
    base = *(volatile uint8_t**)SHARED_RAM; 

    /* To capture an image, the kernel allocates a hunk of memory, writes the
     * physicial address of this allocation to a known location in the PRU shared
     * RAM, and then manually triggers a PRU system event. The PRU detects this
     * event, reads the memory location, reads in the image data and writes it to
     * the memory location, and finally triggers an interrupt which tells the
     * kernel driver that it is done 
     */

    // wait for signal from ARM. The kernel driver will manually trigger
    // system event 24, and we check that event here. We do this instead
    // of routing the interrupt to R31 because we want to use R31 for fast
    // inter-PRU signalling
    while(!(CT_INTC.SECR0 & 1<<24));
    CT_INTC.SICR_bit.STS_CLR_IDX = 24;

    // trigger event 16 to tell PRU0 to start capturing an image
    __R31 = SYS_EVT_16_TRIGGER;

    for(line = 0; line < ROWS; line++) 
    {
      for(chunk = 0; chunk < LINE_CHUNKS; chunk++)
      {
        // wait for the signal from PRU0 and clear after
        while((__R31 & 1U<<31) == 0);
        CT_INTC.SICR_bit.STS_CLR_IDX = 17;

        //transfer chunk to memory
        // TODO this is not perfect because we start the memory transfer as the
        // other core start rewriting this buffer, so this code would likely
        // break at a faster clock speed. We could fix this by just double
        // buffering. However, this is just temporary so we can live with it
        memcpy((char*)base, (char*)shared->buf0, CHUNK_SIZE);

        //offset base address
        base += CHUNK_SIZE;
      }
      /*
      // wait for the signal from PRU0 and clear after
      while((__R31 & 1U<<31) == 0);
      CT_INTC.SICR_bit.STS_CLR_IDX = 17;
      //run_asm1();

      //transfer line to memory
      memcpy((char*)base, (char*)shared->buf, COLS);

      //offset base address
      base += COLS;
      */
    }

    // trigger interrupt to tell kernel transfer is complete
    __R31 = SYS_EVT_20_TRIGGER; // trigger INTC 20
    // TODO do I need to clear this interrupt? I haven't in the past...
  }
}

void initPRU() {
  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  // This is supposed to allow us to write to the global memory space?
  // it works without this, sooo....
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Apparently there is no difference when parallel capture is used!?!?!
  // Parallel Capture Settings
  CT_CFG.GPCFG0_bit.PRU0_GPI_MODE = 0x01; //enable parallel capture
  CT_CFG.GPCFG0_bit.PRU0_GPI_CLK_MODE = 0x01; //capture on positive edge

  // clear all system events
  CT_INTC.SECR0_bit.ENA_STS_31_0 = 0xFFFFFFFF;
  CT_INTC.SECR1_bit.ENA_STS_63_32 = 0xFFFFFFFF;

  /* Something is broken here. No matter what I try, sys events 20-23 will ONLY
   * map to INTC 20-23, while the below mapping says 16-19 should map to INTC
   * 20-23. This makes NO sense according to the documentation. Furthermore,
   * I seem to only be able to get INTC 20-23 to trigger, regardless of what
   * events I trigger(I tried SRSR reg). The mappings seem to have almost no
   * effect. Come back to this.
   */

  // map system event 20 --> chan 2 --> host-2 interrupt. This is used for the 
  // PRU to interrupt the MPU 
  CT_INTC.CMR5_bit.CH_MAP_20 = 2; //sys evt 20 --> chan 2
  CT_INTC.HMR0_bit.HINT_MAP_2 = 2; //chan 2 --> host 2; HOST-2 maps to AM335x interrupt #20

  // map system event 16 --> chan 3 --> Host-0 interrupt --> R31 bit 30 
  CT_INTC.CMR4_bit.CH_MAP_16 = 3; //sys evt 16 --> chan 3
  CT_INTC.HMR0_bit.HINT_MAP_3 = 0; //chan 3 --> host 0

  // map system event 17 --> chan 1 --> Host-1 interrupt --> R31 bit 31
  CT_INTC.CMR4_bit.CH_MAP_17 = 1; //sys evt 17 --> chan 1
  CT_INTC.HMR0_bit.HINT_MAP_1 = 1; //chan 1 --> host 1;

  // point other channels at other interrupt
  CT_INTC.HMR0_bit.HINT_MAP_0 = 0xff; // TODO something is broken with using channel 0
  CT_INTC.HMR1_bit.HINT_MAP_4 = 0xff; 
  CT_INTC.HMR1_bit.HINT_MAP_5 = 0xff; 
  CT_INTC.HMR1_bit.HINT_MAP_6 = 0xff; 
  CT_INTC.HMR1_bit.HINT_MAP_7 = 0xff; 
  CT_INTC.HMR2_bit.HINT_MAP_8 = 0xff; 
  CT_INTC.HMR2_bit.HINT_MAP_9 = 0xff; 

  // enable sys events 16, 17, 20
  CT_INTC.EISR = 16;
  CT_INTC.EISR = 17;
  CT_INTC.EISR = 20;

  // enable host interrupts 0, 1, 2
  CT_INTC.HIEISR = 0;
  CT_INTC.HIEISR = 1;
  CT_INTC.HIEISR = 2;

  // enable global interrupts
  CT_INTC.GER_bit.EN_HINT_ANY |= 0x01; 

  // clear all system events
  CT_INTC.SECR0_bit.ENA_STS_31_0 = 0xFFFFFFFF;
  CT_INTC.SECR1_bit.ENA_STS_63_32 = 0xFFFFFFFF;
}

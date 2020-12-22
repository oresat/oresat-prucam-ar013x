#include "pru_fw.h"

// do_transfer is a function defined in assembly
extern void do_transfer(uint8_t* addr);

void main(void)
{
  // set the shared var to the known address in shared RAM
  struct pru_shared_vars_t* shared = SHARED_VAR_ADDR;

  uint8_t* image_buf;

  // init PRU registers
  initPRU();

  // To capture an image, the kernel allocates a hunk of memory, writes the
  // physicial address of this allocation to a known location in the PRU shared
  // RAM, and then manually triggers a PRU system event. The PRU detects this
  // event, reads the memory location, reads in the image data and writes it to
  // the memory location, and finally triggers an interrupt which tells the
  // kernel driver that it is done 
  while(1)
  {  
    // wait for signal from ARM. The kernel driver will manually trigger
    // system event 24, and we check that event here. We do this instead
    // of routing the interrupt to R31 because we want to use R31 for fast
    // inter-PRU signalling
    while(!(CT_INTC.SECR0 & 1<<24));
    CT_INTC.SICR_bit.STS_CLR_IDX = 24;

    // read the image buffer address from where kernel saved it in shared memory
    image_buf = *(uint8_t**)SHARED_RAM; 

    // Perform the image transfer. This will trigger the other PRU to start the
    // image capture, read the data transfered from the PRU(in the scratchpad),
    // and transfer that data to the image buffer specified by the kernel driver
    do_transfer(image_buf);
  }
}

void initPRU() {
  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Parallel Capture Settings
  CT_CFG.GPCFG0_bit.PRU0_GPI_MODE = 0x01; //enable parallel capture
  CT_CFG.GPCFG0_bit.PRU0_GPI_CLK_MODE = 0x01; //capture on positive edge

  // clear all system events
  CT_INTC.SECR0_bit.ENA_STS_31_0 = 0xFFFFFFFF;
  CT_INTC.SECR1_bit.ENA_STS_63_32 = 0xFFFFFFFF;

  // Something is broken here. No matter what I try, sys events 20-23 will ONLY
  // map to INTC 20-23, while the below mapping says 16-19 should map to INTC
  // 20-23. This makes NO sense according to the documentation. Furthermore,
  // I seem to only be able to get INTC 20-23 to trigger, regardless of what
  // events I trigger(I tried SRSR reg). The mappings seem to have almost no
  // effect. Come back to this.

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
  CT_INTC.EISR = PRU1_TO_PRU0_EVENT;
  CT_INTC.EISR = PRU0_TO_PRU1_EVENT;
  CT_INTC.EISR = PRU_TO_KERNEL_EVENT; // TODO define this

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

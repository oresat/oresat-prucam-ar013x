#include "pru_fw.h"

// image_transfer is a function defined in assembly
extern void image_transfer(uint8_t* addr);

void main(void)
{
  uint8_t* image_buf;

  // init PRU registers
  init_pru();

  // read the image buffer address from where kernel saved it in shared memory
  image_buf = *(uint8_t**)SHARED_RAM; 

  // transfer images forever
  while(1)
  {  
    // Perform the image transfer. This will wait for the trigger from the
    // kernel to start the transfer process. Then it will wait for triggers 
    // from the other PRU, read the data transfered from it(in the scratchpad 
    // registers), and transfer that data to the image buffer specified by the 
    // kernel driver
    image_transfer(image_buf);
  }
}

void init_pru() {
  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Parallel Capture Settings
  CT_CFG.GPCFG0_bit.PRU0_GPI_MODE = 0x01; // enable parallel capture on PRU0
  CT_CFG.GPCFG0_bit.PRU0_GPI_CLK_MODE = 0x01; // capture on positive edge on PRU0
}

#include "pru_fw.h"

// run_asm0 is declared in the *.s assembly file
extern void capture_frame_8b(void);

void main(void)
{
  // set the shared var to the known address in shared RAM
  struct pru_shared_vars_t* shared = SHARED_VAR_ADDR;

  while(1)
  {
    // TODO read streaming state and capture type from shared vars
    capture_frame_8b();
  }
}


#include "pru_fw.h"

// capture_frame_8b is declared in the *.s assembly file
extern void capture_frame_8b(void);

void main(void)
{
  while(1)
  {
    capture_frame_8b();
  }
}


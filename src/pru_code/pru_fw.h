#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <pru_ctrl.h>

#define SHARED_RAM 0x00010000 //offset of PRU shared mem
#define ROWS 960  //rows per image
#define COLS 1280 //pixels per row

// R31 image sync signal bit definitions
#define CLK_BIT 16
#define CLK_MASK 1U<<CLK_BIT
#define VSYNC_BIT 15 
#define VSYNC_MASK 1U<<VSYNC_BIT
#define HSYNC_BIT 14
#define HSYNC_MASK 1U<<HSYNC_BIT

// R31 inter-PRU interrupt bit definitions
#define KERNEL_TO_PRUS_R31_BIT 30 
#define KERNEL_TO_PRUS_R31_MASK 1U<<KERNEL_TO_PRUS_R31_BIT
#define PRU0_TO_PRU1_R31_BIT 31
#define PRU0_TO_PRU1_R31_MASK 1U<<PRU0_TO_PRU1_R31_BIT

// PRU INTC base address constant table offset
#define INTC_CO_TABLE_ENTRY C0
#define SICR_REG_OFFSET 0x24

// PRU system events 
#define KERNEL_TO_PRUS_EVENT 16
#define PRU0_TO_PRU1_EVENT 17
#define PRU1_TO_KERNEL_EVENT 18

// R31 bit 5 enables the interrupt number written to bits 4:0    
#define SYS_EVT_ENABLE 1<<5

// The number written to R31 will trigger the corresponding
// system events 16-31. See TRM section 4.4.1.2.2>
#define SYS_EVT_16_TRIGGER (SYS_EVT_ENABLE | (KERNEL_TO_PRUS_EVENT - 16))
#define SYS_EVT_17_TRIGGER (SYS_EVT_ENABLE | (PRU0_TO_PRU1_EVENT - 16))
#define SYS_EVT_18_TRIGGER (SYS_EVT_ENABLE | (PRU1_TO_KERNEL_EVENT - 16))

#define SCRATCHPAD_BANK_0 10

// number of bytes per transfer chunk
#define CHUNK_SIZE 32

// TODO assert that COLS/CHUNK_SIZE is evenly divisible
// the number of chunks(transfers) per image line 
#define LINE_CHUNKS (COLS / CHUNK_SIZE)

volatile register uint32_t __R30;
volatile register uint32_t __R31;

void init_pru();

// pru_shared_vars_t is a struct that defines variables shared between the PRU
// cores. It must be declared and mapped to a known address in both PRU FWs
struct pru_shared_vars_t {
  uint8_t buf0[COLS]; // shared line buffer
};


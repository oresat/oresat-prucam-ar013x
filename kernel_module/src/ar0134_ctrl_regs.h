/*
 * ===== regs.h =====
 * Define all sets of constant register sets here i.e.
 * startup register, sleep registers, etc.
 */

#ifndef AR0134_CTRL_REGS_H
#define AR0134_CTRL_REGS_H 

#include "cam_i2c.h"

#ifndef CAM_I2C_ADDR
#define CAM_I2C_ADDR 0x10
#endif

//represents the address of the AR0134 image sensor
struct i2c_board_info ar0134_i2c_info __initdata = {
    I2C_BOARD_INFO("AR013X", CAM_I2C_ADDR),
};

// startup regs for AR0134
camera_regs_t ar0134_startupRegs[] = {

  // from "Column Correction ReTriggering" settings in AR0134REV1.ini
  {.reg = 0x301A, .val = 0x10D8}, // disable streaming
  {.reg = 0x30D4, .val = 0x6007}, // disable column correction
  {.reg = 0x0000, .val = 200},    // delay 100ms
  {.reg = 0x301A, .val = 0x10DC}, // enable streaming
  {.reg = 0x0000, .val = 200},    // delay 100ms
  {.reg = 0x301A, .val = 0x10D8}, // disable streaming
  {.reg = 0x30D4, .val = 0xE007}, // enable column correction
  {.reg = 0x0000, .val = 200},    // delay 100ms
  {.reg = 0x301A, .val = 0x10DC}, // enable streaming

  // init settings from Arducam AR0134 config
  {.reg = 0x3028, .val = 0x0010}, 		// ROW_SPEED = 16

  // 12.5MHz settings
  {.reg = 0x302E, .val = 0x0009}, 		// PRE_PLL_CLK_DIV
  {.reg = 0x3030, .val = 0x0096}, 		// PLL_MULTIPLIER
  {.reg = 0x302C, .val = 0x0009}, 		// VT_SYS_CLK_DIV
  {.reg = 0x302A, .val = 0x0004}, 		// VT_PIX_CLK_DIV

  {.reg = 0x3032, .val = 0x0000}, 		// DIGITAL_BINNING = 0
  {.reg = 0x30B0, .val = 0x0080}, 		// DIGITAL_TEST = 128
  {.reg = 0x301A, .val = 0x10DC},  		// RESET_REGISTER = 4312
  {.reg = 0x300C, .val = 0x0672},  		// LINE_LENGTH_PCK = 1650
  {.reg = 0x3002, .val = 0x0000},  		// Y_ADDR_START = 0
  {.reg = 0x3004, .val = 0x0000},  		// X_ADDR_START = 0
  {.reg = 0x3006, .val = 0x03BF},  		// Y_ADDR_END = 959
  {.reg = 0x3008, .val = 0x04FF},  		// X_ADDR_END = 1279
  {.reg = 0x300A, .val = 0x03DE},  		// FRAME_LENGTH_LINES = 990
  {.reg = 0x3012, .val = 0x0180},  		// COARSE_INTEGRATION_TIME
  {.reg = 0x3014, .val = 0x00C0},  		// FINE_INTEGRATION_TIME = 192
  {.reg = 0x30A6, .val = 0x0001},  		// Y_ODD_INC = 1
  {.reg = 0x308C, .val = 0x0000},  		// Y_ADDR_START_CB = 0
  {.reg = 0x308A, .val = 0x0000},  		// X_ADDR_START_CB = 0
  {.reg = 0x3090, .val = 0x03BF},  		// Y_ADDR_END_CB = 959
  {.reg = 0x308E, .val = 0x04FF},  		// X_ADDR_END_CB = 1279
  {.reg = 0x30AA, .val = 0x03DE},  		// FRAME_LENGTH_LINES_CB = 990
  {.reg = 0x3016, .val = 0x0064},  		// COARSE_INTEGRATION_TIME_CB = 100
  {.reg = 0x3018, .val = 0x00C0},  		// FINE_INTEGRATION_TIME_CB = 192
  {.reg = 0x30A8, .val = 0x0001},  		// Y_ODD_INC_CB = 1
  {.reg = 0x3040, .val = 0x0000},  		// READ_MODE = 0
  {.reg = 0x3064, .val = 0x1982},  		// EMBEDDED_DATA_CTRL = 6530
  {.reg = 0x31C6, .val = 0x8000},  		// HISPI_CONTROL_STATUS = 32768 (DEFAULT)
};
#endif

/**
 * @file    ar0134_ctrl_regs.h
 * @brief   AR0134 CMOS Digital Image Sensor register startup sequence.
 *
 * @addtogroup AR013x
 */

#ifndef AR0134_CTRL_REGS_H
#define AR0134_CTRL_REGS_H 

#include "ar013x_regs.h"
#include "cam_i2c.h"

#ifndef CAM_I2C_ADDR
#define CAM_I2C_ADDR 0x10
#endif

/** @brief Represents the address of the AR013X image sensor */
struct i2c_board_info ar0134_i2c_info __initdata = {
    I2C_BOARD_INFO("AR013X", CAM_I2C_ADDR),
};

/** @brief startup registers for AR0134 CMOS Digital Image Sensor */
camera_regs_t ar0134_startupRegs[] = {
    {AR013X_AD_RESET_REGISTER, 0x0001},
    {0x0000, 0x0064}, // delay
    {AR013X_AD_RESET_REGISTER, 0x10D8}, //Disable Serial, Enable Parallel, Drive Outputs(no hi-z), lock reg

    // from "Column Correction ReTriggering" settings in AR013XREV1.ini
    {AR013X_AD_RESET_REGISTER, 0x10D8}, // disable streaming
    {AR013X_AD_COLUMN_CORRECTION, 0x6007},
    {0x0000, 200}, // delay
    {AR013X_AD_RESET_REGISTER, 0x10DC}, // enable streaming
    {0x0000, 200}, // delay
    {AR013X_AD_RESET_REGISTER, 0x10D8}, // disable streaming
    {AR013X_AD_COLUMN_CORRECTION, 0xE007},
    {0x0000, 200},// delay
    {AR013X_AD_RESET_REGISTER, 0x10DC}, // enable streaming

    {AR013X_AD_ROW_SPEED, 0x0010},

    // TODO not sure what this is for, look into that
    {AR013X_AD_OPERATION_MODE_CTRL, 0x0029},

    // from AR013XREV1.ini in ON Semi software suite,
    // "[AR013X Rev1 Optimized settings DCDS 5-7-2013]"
    // black magic registers
    {AR013X_AD_DATA_PEDESTAL, 0x00C8},
    {0x3EDA, 0x0F03}, // DAC_LD_14_15
    {0x3EDE, 0xC005}, // DAC_LD_18_19
    {0x3ED8, 0x09EF}, // DAC_LD_12_13
    {0x3EE2, 0xA46B}, // DAC_LD_22_23
    {0x3EE0, 0x057D}, // DAC_LD_20_21
    {0x3EDC, 0x0080}, // DAC_LD_16_17
    {AR013X_AD_DARK_CONTROL, 0x0404},
    {0x3EE6, 0x4303}, // DAC_LD_26_27
    {0x3EE4, 0xD208}, // DAC_LD_24_25
    {0x3ED6, 0x00BD}, // DAC_LD_10_11
    {AR013X_AD_DIGITAL_TEST, 0x1300},
    {AR013X_AD_DIGITAL_CTRL, 0x0018},
    {AR013X_AD_COLUMN_CORRECTION, 0xE007},

    // power up sequence 7 - 45MHz settings
    {AR013X_AD_PRE_PLL_CLK_DIV, 0x000A},
    {AR013X_AD_PLL_MULTIPLIER, 0x00CB},
    {AR013X_AD_VT_SYS_CLK_DIV, 0x0004},
    {AR013X_AD_VT_PIX_CLK_DIV, 0x0003},

    // power up sequence 8 - wait for PLL to lock
    {0x0000, 0x0001}, // delay 1 ms

    {AR013X_AD_DIGITAL_BINNING, 0x0000},
    {AR013X_AD_DIGITAL_TEST, 0x0080},
    {AR013X_AD_RESET_REGISTER, 0x10DC},
    {AR013X_AD_LINE_LENGTH_PCK, 0x0672},

    // context A
    {AR013X_AD_Y_ADDR_START, 0x0000}, // y start = 0 
    {AR013X_AD_X_ADDR_START, 0x0000}, // x start = 0
    {AR013X_AD_Y_ADDR_END, 0x03BF}, // y end = 959
    {AR013X_AD_X_ADDR_END, 0x04FF}, // x end = 1279
    {AR013X_AD_FRAME_LEN_LINES, 0x03DE},
    {AR013X_AD_COARSE_INTEGRATION_TIME, 0x0180},
    {AR013X_AD_FINE_INTERGRATION_TIME, 0x00C0},
    {AR013X_AD_Y_ODD_INC, 0x0001},

    // context B
    {AR013X_AD_Y_ADDR_START_CB, 0x0000},
    {AR013X_AD_X_ADDR_START_CB, 0x0000},
    {AR013X_AD_Y_ADDR_END_CB, 0x03BF},
    {AR013X_AD_X_ADDR_END_CB, 0x04FF},
    {AR013X_AD_FRAME_LEN_LINES_CB, 0x03DE},
    {AR013X_AD_COARSE_INTEGRATION_TIME_CB, 0x0064},
    {AR013X_AD_FINE_INTERGRATION_TIME_CB, 0x00C0},
    {AR013X_AD_Y_ODD_INC_CB, 0x0001},

    {AR013X_AD_READ_MODE, 0x0000},
    {AR013X_AD_HIPSI_CONTROL_STATUS, 0x8000},

    {AR013X_AD_DIGITAL_TEST, 0x1300},
    {0x0000, 0x0064}, // delay
    {AR013X_AD_AE_CTRL_REG, 0x0000},
    {AR013X_AD_EMBEDDED_DATA_CTRL, 0x1982}, // enable embedded data, this is needed for auto exposure
    {AR013X_AD_TEST_PATTERN_MODE, 0x0000},

    // power up sequence 9 - enable streaming
    {AR013X_AD_RESET_REGISTER, 0x10DC},  //Ensable Serial, Enable Parallel, Drive Outputs(no hi-z), lock reg, streaming mode(not low power)
    {0x0000, 0x0064}, // delay
    {0x0000, 0x0000}, // Zeros mark end of sequence
};

#endif /* AR0134_CTRL_REGS_H */

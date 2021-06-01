/**
 * @file    ar0130_ctrl_regs.h
 * @brief   AR0130 CMOS Digital Image Sensor register startup sequence.
 *
 * @addtogroup AR013x
 */

#ifndef AR0130_CTRL_REGS_H
#define AR0130_CTRL_REGS_H

#include "ar013x_regs.h"
#include "cam_i2c.h"

#ifndef CAM_I2C_ADDR
#define CAM_I2C_ADDR 0x10
#endif

/** @breif Represents the address of the AR013X image sensor */
struct i2c_board_info ar013x_i2c_info __initdata = {
    I2C_BOARD_INFO("AR013X", CAM_I2C_ADDR),
};

/** @brief startup registers for AR0130 CMOS Digital Image Sensor */
camera_regs_t ar0130_startupRegs[] = {
    {AR013X_AD_RESET_REGISTER, 0x0001},
    {0x0000, 0x0064},                   // delay
    {AR013X_AD_RESET_REGISTER, 0x10D8}, // Disable Serial, Enable Parallel,
                                        // Drive Outputs(no hi-z), lock reg

    // from AR0130REV1.ini in ON Semi software suite,
    // "[AR0130 Rev1 DCDS sequencer load 5-07-2013 300 pixclks]"
    {AR013X_AD_SEQ_CTRL_PORT, 0x8000},
    {AR013X_AD_SEQ_DATA_PORT, 0x0225},
    {AR013X_AD_SEQ_DATA_PORT, 0x5050},
    {AR013X_AD_SEQ_DATA_PORT, 0x2D26},
    {AR013X_AD_SEQ_DATA_PORT, 0x0828},
    {AR013X_AD_SEQ_DATA_PORT, 0x0D17},
    {AR013X_AD_SEQ_DATA_PORT, 0x0926},
    {AR013X_AD_SEQ_DATA_PORT, 0x0028},
    {AR013X_AD_SEQ_DATA_PORT, 0x0526},
    {AR013X_AD_SEQ_DATA_PORT, 0xA728},
    {AR013X_AD_SEQ_DATA_PORT, 0x0725},
    {AR013X_AD_SEQ_DATA_PORT, 0x8080},
    {AR013X_AD_SEQ_DATA_PORT, 0x2917},
    {AR013X_AD_SEQ_DATA_PORT, 0x0525},
    {AR013X_AD_SEQ_DATA_PORT, 0x0040},
    {AR013X_AD_SEQ_DATA_PORT, 0x2702},
    {AR013X_AD_SEQ_DATA_PORT, 0x1616},
    {AR013X_AD_SEQ_DATA_PORT, 0x2706},
    {AR013X_AD_SEQ_DATA_PORT, 0x2117},
    {AR013X_AD_SEQ_DATA_PORT, 0x3626},
    {AR013X_AD_SEQ_DATA_PORT, 0xA717},
    {AR013X_AD_SEQ_DATA_PORT, 0x0326},
    {AR013X_AD_SEQ_DATA_PORT, 0xA717},
    {AR013X_AD_SEQ_DATA_PORT, 0x1F28},
    {AR013X_AD_SEQ_DATA_PORT, 0x051A},
    {AR013X_AD_SEQ_DATA_PORT, 0x174A},
    {AR013X_AD_SEQ_DATA_PORT, 0x26E7},
    {AR013X_AD_SEQ_DATA_PORT, 0x175A},
    {AR013X_AD_SEQ_DATA_PORT, 0x26E6},
    {AR013X_AD_SEQ_DATA_PORT, 0x1703},
    {AR013X_AD_SEQ_DATA_PORT, 0x26E4},
    {AR013X_AD_SEQ_DATA_PORT, 0x174B},
    {AR013X_AD_SEQ_DATA_PORT, 0x2700},
    {AR013X_AD_SEQ_DATA_PORT, 0x1710},
    {AR013X_AD_SEQ_DATA_PORT, 0x1D17},
    {AR013X_AD_SEQ_DATA_PORT, 0xFF17},
    {AR013X_AD_SEQ_DATA_PORT, 0x2026},
    {AR013X_AD_SEQ_DATA_PORT, 0x6017},
    {AR013X_AD_SEQ_DATA_PORT, 0x0125},
    {AR013X_AD_SEQ_DATA_PORT, 0x2020},
    {AR013X_AD_SEQ_DATA_PORT, 0x1721},
    {AR013X_AD_SEQ_DATA_PORT, 0x2500},
    {AR013X_AD_SEQ_DATA_PORT, 0x2021},
    {AR013X_AD_SEQ_DATA_PORT, 0x1710},
    {AR013X_AD_SEQ_DATA_PORT, 0x2805},
    {AR013X_AD_SEQ_DATA_PORT, 0x1B17},
    {AR013X_AD_SEQ_DATA_PORT, 0x0327},
    {AR013X_AD_SEQ_DATA_PORT, 0x0617},
    {AR013X_AD_SEQ_DATA_PORT, 0x0317},
    {AR013X_AD_SEQ_DATA_PORT, 0x4126},
    {AR013X_AD_SEQ_DATA_PORT, 0x6017},
    {AR013X_AD_SEQ_DATA_PORT, 0xAE25},
    {AR013X_AD_SEQ_DATA_PORT, 0x0090},
    {AR013X_AD_SEQ_DATA_PORT, 0x2700},
    {AR013X_AD_SEQ_DATA_PORT, 0x2618},
    {AR013X_AD_SEQ_DATA_PORT, 0x2800},
    {AR013X_AD_SEQ_DATA_PORT, 0x2E2A},
    {AR013X_AD_SEQ_DATA_PORT, 0x2808},
    {AR013X_AD_SEQ_DATA_PORT, 0x1E08},
    {AR013X_AD_SEQ_DATA_PORT, 0x3114},
    {AR013X_AD_SEQ_DATA_PORT, 0x4040},
    {AR013X_AD_SEQ_DATA_PORT, 0x1420},
    {AR013X_AD_SEQ_DATA_PORT, 0x2014},
    {AR013X_AD_SEQ_DATA_PORT, 0x1010},
    {AR013X_AD_SEQ_DATA_PORT, 0x3414},
    {AR013X_AD_SEQ_DATA_PORT, 0x0010},
    {AR013X_AD_SEQ_DATA_PORT, 0x1400},
    {AR013X_AD_SEQ_DATA_PORT, 0x2014},
    {AR013X_AD_SEQ_DATA_PORT, 0x0040},
    {AR013X_AD_SEQ_DATA_PORT, 0x1318},
    {AR013X_AD_SEQ_DATA_PORT, 0x0214},
    {AR013X_AD_SEQ_DATA_PORT, 0x7070},
    {AR013X_AD_SEQ_DATA_PORT, 0x0414},
    {AR013X_AD_SEQ_DATA_PORT, 0x7070},
    {AR013X_AD_SEQ_DATA_PORT, 0x0314},
    {AR013X_AD_SEQ_DATA_PORT, 0x7070},
    {AR013X_AD_SEQ_DATA_PORT, 0x1720},
    {AR013X_AD_SEQ_DATA_PORT, 0x0214},
    {AR013X_AD_SEQ_DATA_PORT, 0x0020},
    {AR013X_AD_SEQ_DATA_PORT, 0x0214},
    {AR013X_AD_SEQ_DATA_PORT, 0x0050},
    {AR013X_AD_SEQ_DATA_PORT, 0x0414},
    {AR013X_AD_SEQ_DATA_PORT, 0x0020},
    {AR013X_AD_SEQ_DATA_PORT, 0x0414},
    {AR013X_AD_SEQ_DATA_PORT, 0x0050},
    {AR013X_AD_SEQ_DATA_PORT, 0x2203},
    {AR013X_AD_SEQ_DATA_PORT, 0x1400},
    {AR013X_AD_SEQ_DATA_PORT, 0x2003},
    {AR013X_AD_SEQ_DATA_PORT, 0x1400},
    {AR013X_AD_SEQ_DATA_PORT, 0x502C},
    {AR013X_AD_SEQ_DATA_PORT, 0x2C2C},
    // black magic registers
    {0x309E, 0x0000}, // RESERVED // DCDS_PROG_START_ADDR
    {0x30E4, 0x6372}, // RESERVED // ADC_BITS_6_7
    {0x30E2, 0x7253}, // RESERVED // ADC_BITS_4_5
    {0x30E0, 0x5470}, // RESERVED // ADC_BITS_2_3
    {0x30E6, 0xC4CC}, // RESERVED // ADC_CONFIG1
    {0x30e8, 0x8050}, // RESERVED // ADC_CONFIG2

    {0x0000, 200}, // delay 200 ms

    // TODO not sure what this is for, look into that
    {AR013X_AD_OPERATION_MODE_CTRL, 0x0029}, // OP MODE CTL

    // from AR0130REV1.ini in ON Semi software suite,
    // "[AR0130 Rev1 Optimized settings DCDS 5-7-2013]"
    // more black magic registers
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

    // TODO pick up here, ini file line 449, looks like there is some reset
    // stuff

    {AR013X_AD_COARSE_INTEGRATION_TIME, 0x00c0},
    {AR013X_AD_DIGITAL_BINNING, 0x0000},
    {AR013X_AD_Y_ADDR_START, 0x0000}, // y start = 0
    {AR013X_AD_X_ADDR_START, 0x0000}, // x start = 0
    {AR013X_AD_Y_ADDR_END, 0x03BF},   // y end = 959
    {AR013X_AD_X_ADDR_END, 0x04FF},   // x end = 1279
    {AR013X_AD_FRAME_LEN_LINES, 0x03DE},

    {AR013X_AD_LINE_LENGTH_PCK, 0x0672},
    {AR013X_AD_RESET_REGISTER, 0x10D8},
    {AR013X_AD_HDR_COMP, 0x0001},

    // power up sequence 7 - 45MHz settings
    {AR013X_AD_PRE_PLL_CLK_DIV, 0x000A},
    {AR013X_AD_PLL_MULTIPLIER, 0x00CB},
    {AR013X_AD_VT_SYS_CLK_DIV, 0x0004},
    {AR013X_AD_VT_PIX_CLK_DIV, 0x0003},

    // power up sequence 8 - wait for PLL to lock
    {0x0000, 0x0001}, // delay 1 ms

    {AR013X_AD_DIGITAL_TEST, 0x1300},
    {0x0000, 0x0064}, // delay 100 ms
    {AR013X_AD_AE_CTRL_REG, 0x0000},
    {AR013X_AD_EMBEDDED_DATA_CTRL,
     0x1982}, // enable embedded data, this is needed for auto exposure
    {AR013X_AD_TEST_PATTERN_MODE, 0x0000},

    // power up sequence 9 - enable streaming
    {AR013X_AD_RESET_REGISTER,
     0x10DC}, // disable Serial, Enable Parallel, Drive Outputs(no hi-z), lock
              // reg, streaming mode(not low power)
    {0x0000, 0x0064}, // delay 100 ms
    {0x0000, 0x0000}, // Zeros mark end of sequence
};

#endif

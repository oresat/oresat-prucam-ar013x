#ifndef CAM_I2C_H
#define CAM_I2C_H

#include <linux/i2c.h>

typedef struct {
    /** the register number */
    uint16_t reg;
    /** the value to write to register */
    uint16_t val;
} camera_regs_t;

/**
 * @breif Initialize the camera registers
 * @param i2c_info
 * @return 0 on success or non-zero on error
 */
int init_cam_i2c(struct i2c_board_info i2c_info);

/**
 * @breif Initialize the camera registers
 * @return 0 on success or non-zero on error
 */
int end_cam_i2c(void);

/**
 * @breif Initialize the camera registers
 * @return 0 on success and negative errno value on error
 */
int init_camera_regs(camera_regs_t *regs);

/**
 * @breif Writes the 16bit value from a 16bit camera register
 * @param reg The Register to write to
 * @param val The value to write
 * @return 0 on success and negative errno value on error
 */
int write_cam_reg(uint16_t reg, uint16_t val);

/**
 * @breif Reads a 16bit value from a 16bit camera register
 * @param reg The Register to read from
 * @param val A pointer to read the value to
 * @return 0 on success and negative errno value on error
 */
int read_cam_reg(uint16_t reg, uint16_t *val);

#endif

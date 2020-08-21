
#ifndef CAM_I2C_H
#define CAM_I2C_H

#include <linux/i2c.h>

typedef struct {
    /** the register number */
    uint16_t reg;
    /** the value to write to register */
    uint16_t val;
} camera_regs_t;


int init_cam_i2c(struct i2c_board_info i2c_info);
int end_cam_i2c(void);

int init_camera_regs(camera_regs_t *regs);
int write_cam_reg(uint16_t reg, uint16_t val);
int read_cam_reg(uint16_t reg, uint16_t *val);

#endif

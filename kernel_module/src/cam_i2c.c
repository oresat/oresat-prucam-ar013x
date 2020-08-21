#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include "cam_i2c.h"


struct i2c_adapter* i2c_adap;

/** i2c_client is used to interact with the image sensor's i2c slave address */
struct i2c_client* client;


int init_cam_i2c(struct i2c_board_info i2c_info) {
    i2c_adap = i2c_get_adapter(2);

    client = i2c_new_device(i2c_adap, &i2c_info);

    if(client == NULL)
        return -1; // TODO real ERRNO value

    return 0;
}

int end_cam_i2c(void) {
    i2c_unregister_device(client);
    return 0;
}


int init_camera_regs(camera_regs_t *regs){
    int ret;

    if(regs == NULL)
        return 0; // nothing todo

    for(int i = 0; ; i++){
        // last entry in the array will be empty
        if(regs[i].reg == 0 && regs[i].val == 0)
            break;

        // if reg == 0, then we delay val milliseconds
        if(regs[i].reg == 0) {
            // delay is more wasteful than sleep, but the resultant traffic is
            // cleaner. TODO Come back to this.
            mdelay(regs[i].val); //msleep(reg.val);
            continue;
        }

        ret = write_cam_reg(regs[i].reg, regs[i].val);
        if(ret < 0) {
            printk("ERROR: i2c write reg: %04x val: %04x returned: %d\n",regs[i].reg, regs[i].val, ret);
            // return ret;
        }
        // mdelay(1);
    }

    return 0;
}


/**
 * write_cam_reg write the value to the specified register. The AR013X has 16
 * bit register addresses and values, which differs from "normal" i2c
 */
int write_cam_reg(uint16_t reg, uint16_t val) {
    int ret;

    //unpack 16 bit values in buffer of bytes
    char buf[4] = {
        reg >> 8,
        reg,
        val >> 8,
        val,
    };

    // TODO should I check that num bytes matches?
    ret = i2c_master_send(client, buf, 4);
    if(ret < 0)
        return ret;

    return 0;
}


/**
 * write_cam_reg write the value to the specified register. The AR013X has 16
 * bit register addresses and values, which differs from "normal" i2c
 */
int read_cam_reg(uint16_t reg, uint16_t *val) {
    int ret;
    char buf[4] = {
        reg >> 8,
        reg,
        0,
        0,
    };

    if(val == NULL)
        return -1;

    ret = i2c_master_recv(client, buf, 4);
    if(ret < 0)
        return ret;

    memcpy(val, buf, sizeof(uint16_t));

    return 0;
}

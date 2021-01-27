#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include "cam_i2c.h"


/** The i2c adapter */
struct i2c_adapter* i2c_adap;
/** Used to interact with the image sensor's i2c slave address */
struct i2c_client* client;


int init_cam_i2c(struct i2c_board_info i2c_info) {
    int ret = 0;

    i2c_adap = i2c_get_adapter(2);

    client = i2c_new_device(i2c_adap, &i2c_info);
    if(client == NULL) {
        printk(KERN_ERR "i2c register failed\n");
        ret = -ENXIO; 
    }

    return ret;
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
            printk(KERN_ERR "i2c write reg: %04x val: %04x returned: %d\n",regs[i].reg, regs[i].val, ret);
            // return ret;
        }
        // mdelay(1);
    }

    return 0;
}


int write_cam_reg(uint16_t reg, uint16_t val) {
    int ret;

    //unpack 16 bit values in buffer of bytes
    char buf[4] = {
        reg >> 8,
        reg,
        val >> 8,
        val,
    };

    ret = i2c_master_send(client, buf, 4);
    if(ret < 0) {
      	printk(KERN_INFO "I2C write for reg %x failed with %d", reg, ret);
    } else if(ret == 0) {
      	printk(KERN_INFO "No data was written for reg %x", reg);
        return -EBADMSG;
    }
        
    return 0;
}


int read_cam_reg(uint16_t reg, uint16_t *val) {
    int ret;

    // safety first
    if(val == NULL)
        return -EINVAL;

    // I2C send the little byte first, which is sorta big-endian compared
    // to the little endian uint16 arguments. Convert these values to BE
    __be16 reg_be = cpu_to_be16(reg);
    __be16 val_be = 0;

    // construct the i2c message that:
    // - writes a 16 bit register (to be read)
    // - reads back the 16 bit contents of that register
    //
    // This is to be used with i2c_transfer() and not i2c_master_recv() 
    // because i2c_master_recv does not support reading 16 bit registers
    struct i2c_msg msg[] = {
        // Write the 16bit reg. I2C_M_REV_DIR_ADDR is for the need to trick
        // the i2c driver to think its receiving while it does a write. 
    	{.addr=0x10, .flags=I2C_M_REV_DIR_ADDR, .len=2, .buf=(char*)&reg_be},

	// Receive the 16bit value. The I2C_M_RD is for receiving
    	{.addr=0x10, .flags=I2C_M_RD, .len=2, .buf=(char*)&val_be},
    };

    // start the i2c tranfers and check the return val
    ret = i2c_transfer(i2c_adap, msg, 2);
    if(ret < 0) {
        printk(KERN_ERR "I2C read at reg %x failed with error code %d", reg, ret);
        return ret;
    } else if (ret != 2) {
        // make sure 2 bytes we read
        printk(KERN_ERR "I2C read at reg %x read incorrect number of bytes %d", reg, ret);
        return -EBADMSG;
    }

    // convert the BE value we read to LE and save to argument pointer
    *val = be16_to_cpu(val_be);

    printk(KERN_DEBUG "I2C read for reg 0x%x is 0x%x", reg, *val);

    return 0;
}

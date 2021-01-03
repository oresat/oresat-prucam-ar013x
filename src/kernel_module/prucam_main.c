#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include "ar0130_ctrl_regs.h"
#include "ar0134_ctrl_regs.h"
#include "cam_gpio.h"
#include "cam_i2c.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oliver Rew");
MODULE_DESCRIPTION("Interface to a PRU and a camera");
MODULE_VERSION("0.2.2");


#define DEVICE_NAME     "prucam"    // The device will appear at /dev/prucam 
#define CLASS_NAME      "pru"       // The device class -- this is a character device driver
#define ROWS            960
#define COLS            1280
#define PIXELS          (ROWS * COLS)
#define PRUBASE         0x4a300000
#define PRUSHAREDRAM    (PRUBASE + 0x10000)
#define PRUINTC_OFFSET  0x20000
#define SRSR0_OFFSET    0x200
#define NUM_IRQS        8
#define CONTEXT_A       0
#define CONTEXT_B       1


static uint16_t get_reg(const char *attr);


/** Stores the device number -- determined automatically */
static int majorNumber;
/** The device-driver class struct pointer */
static struct class* prucamClass = NULL;
/** The device-driver device struct pointer */
static struct device* prucamDevice = NULL;
/** the current context */
static int context = CONTEXT_A;
uint16_t digital_test = 0x1300;
uint16_t digital_binning = 0;
DEFINE_MUTEX(cam_mtx);


static ssize_t prucam_context_show(struct device *dev, struct device_attribute *attr, char *buf) {
    uint16_t value = 0, reg;
    int len = 0;

    reg = get_reg(attr->attr.name);

    read_cam_reg(reg, &value);

    if(strcmp(attr->attr.name, "context") == 0){
        // Context switch bit is 13 in reg 0x30B0
        //read_cam_reg(0x30B0, &value);
        value = digital_test;
        value &= 0xFDFF; // TODO remove this
        value >>= 13;
        value &= 0x1;
    }
    else if (strcmp(attr->attr.name, "x_size") == 0) {
        // reg is for x_attr_end
        value += 1; // x_attr_start is 0
    }
    else if (strcmp(attr->attr.name, "y_size") == 0) {
        // reg is for y_attr_end
        value -= 1; // y_attr_start is 2
    }
    else if (strcmp(attr->attr.name, "analog_gain") == 0) {
        /** 2 bits in reg 0x30B0, Context A is [5:4] & Context B is [9:8] */
        value = digital_test; //TODO remove 
        value >>= context == CONTEXT_A ? 4 : 8;
        value &= 0x3;
        digital_test = value; // set global
    }
    else if (strcmp(attr->attr.name, "digital_binning") == 0) {
        /** 2 bits in reg 0x3032, Context A is [1:0] & Context B is [5:4] */
        value = digital_binning;
        if(context == CONTEXT_B)
            value >>= 4;
        value &= 0x3;
    }

    len = sprintf(buf, "%d\n", (int)value);

    if (len <= 0)
        dev_err(dev, "prucam: Invalid sprintf len: %d\n", len);

    return len;
}


static ssize_t prucam_context_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    int temp, r;
    uint16_t value, reg = 0;

    if((r = kstrtoint(buf, 10, &temp)) < 0)
        return count;

    value = (uint16_t)temp;

    reg = get_reg(attr->attr.name);
    
    if(strcmp(attr->attr.name, "context") == 0) {
        // Context switch bit is 13 in reg 0x30B0
        value <<= 13;
        value &= 0x0200;
        digital_test &= 0xFDFF;
        value |= digital_binning;
        digital_test = value; // set global
    }
    else if (strcmp(attr->attr.name, "x_size") == 0) {
        // reg is for x_attr_end
        value -= 1; // x_attr_start is 0
    }
    else if (strcmp(attr->attr.name, "y_size") == 0) {
        // reg is for y_attr_end
        value += 1; // y_attr_start is 2
    }
    else if (strcmp(attr->attr.name, "analog_gain") == 0) {
        /** 2 bits in reg 0x30B0, Context A is [5:4] & Context B is [9:8] */
        if(context == CONTEXT_A) {
            value <<= 4;
            value &= 0x0030;
            digital_test &= 0xFFCF;
        }
        else {
            value <<= 8;
            value &= 0x0300;
            digital_test &= 0xFCFF;
        }
        value |= digital_test;
        digital_test = value; // set global
    }
    else if (strcmp(attr->attr.name, "digital_binning") == 0) {
        /** 2 bits in reg 0x3032, Context A is [1:0] & Context B is [5:4] */
        if(context == CONTEXT_A) {
            value &= 0x0003;
            digital_binning &= 0xFFFC;
        }
        else {
            value <<= 4;
            value &= 0x0030;
            digital_binning &= 0xFFCF;
        }
        value |= digital_binning;
        digital_binning = value; // set global
    }

    write_cam_reg(reg, value);

    return count;
}


static DEVICE_ATTR(context, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(x_size, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(y_size, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(coarse_time, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(fine_time, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(y_odd_inc, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(green1_gain, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(blue_gain, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(red_gain, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(green2_gain, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(global_gain, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(analog_gain, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(frame_len_lines, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);
static DEVICE_ATTR(digital_binning, S_IRUGO | S_IWUSR, prucam_context_show, prucam_context_store);


static int      dev_open(struct inode *, struct file *);
static int      dev_release(struct inode *, struct file *);
static ssize_t  dev_read(struct file *, char *, size_t, loff_t *);
static int      pru_handshake(int physAddr);
static irq_handler_t    irqhandler(unsigned int irq, void *dev_id, struct pt_regs *regs);
int             pru_probe(struct platform_device*);
int             pru_remove(struct platform_device*);
static void     free_irqs(void);


/**
 * TODO - dedicated error interrupt line from PRU that triggers handler to shut down
 *   other opperations
 */
static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .release = dev_release,
};


//Memory pointers
dma_addr_t dma_handle = (dma_addr_t)NULL;
int *cpu_addr = NULL;
int* physAddr = NULL;
volatile int int_triggered = 0;


static const struct of_device_id my_of_ids[] = {
    { .compatible = "prudev,prudev" },
    { },
};


static struct platform_driver prudrvr = {
    .driver = {
        .name = "prudev",
        .owner = THIS_MODULE,
        .of_match_table = my_of_ids
    },
    .probe = pru_probe,
    .remove = pru_remove,
};


typedef struct {
    char* name;
    int   num;
}irq_info;


/**
 * mapping of an irq name(from the platfor device in the device device-tree)
 * to it's assigned linux irq number(in /proc/interrupt). When the irq is 
 * successfully retrieved and requested, the 'num' will be assigned.
 */
irq_info irqs[] = {
    {"20", -1},
    {"21", -1},
    {"22", -1},
    {"23", -1},
    {"24", -1},
    {"25", -1},
    {"26", -1},
    {"27", -1}
};


int pru_probe(struct platform_device* dev) {
    int ret = 0; // return value
    int irq;
    printk(KERN_INFO "prucam: probing %s\n", dev->name);

    if(prucamDevice == NULL){
        printk(KERN_ERR "prucamDevice NULL!\n");
        return -1;
    }

    for(int i = 0 ; i < NUM_IRQS ; i++) {
        irq = platform_get_irq_byname(dev, irqs[i].name);
        printk(KERN_INFO "prucam: platform_get_irq(%s) returned: %d\n", irqs[i].name, irq);
        if(irq < 0)
            return irq;

        ret = request_irq(irq, (irq_handler_t)irqhandler, IRQF_TRIGGER_RISING, "prudev", NULL);
        printk(KERN_INFO "prucam: request_irq(%d) returned: %d\n", irq, ret);
        if(ret < 0)
            return ret;

        irqs[i].num = irq;
    }

    //set DMA mask
    ret = dma_set_coherent_mask(prucamDevice, 0xffffffff);
    if(ret != 0) {
        printk(KERN_INFO "Failed to set DMA mask : error %d\n", ret);
        return 0;
    }

    /**
     * I initial used GFP_DMA flag below, but I could not allocate >1 MiB
     * I am unsure what the ideal flags for this are, but GFP_KERNEL seems to
     * work
     */
    cpu_addr = dma_alloc_coherent(prucamDevice, PIXELS, &dma_handle, GFP_KERNEL);
    if(cpu_addr == NULL) {
        printk(KERN_INFO "Failed to allocate memory\n");
        return -1;
    }

    printk(KERN_INFO "prucam: virtual Address: %x\n", (int)cpu_addr);

    physAddr = (int*)dma_handle;
    printk(KERN_INFO "prucam: physical Address: %x\n", (int)physAddr);
    int_triggered = 0;

    return 0;
}


int pru_remove(struct platform_device* dev) {
    // free irqs alloc'd in the probe
    free_irqs();

    dma_free_coherent(prucamDevice, PIXELS, cpu_addr, dma_handle);
    printk(KERN_INFO "prucam: Freed %d bytes\n", PIXELS); 

    return 0;
}


/** 
 * @brief The LKM initialization function
 * The static keyword restricts the visibility of the function to within this C file. The __init
 * macro means that for a built-in driver (not a LKM) the function is only used at initialization
 * time and that it can be discarded and its memory freed up after that point.
 * @return returns 0 if successful
 */
static int __init prucam_init(void) {
    camera_regs_t *startupRegs = NULL;
    uint16_t cam_ver = 0;
    int regDrvr, r;

    printk(KERN_INFO "prucam: Initializing the prucam v1\n");

    // Try to dynamically allocate a major number for the device -- more difficult but worth it
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if(majorNumber<0){
        printk(KERN_ALERT "prucam failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "prucam: registered correctly with major number %d\n", majorNumber);

    // Register the device class
    prucamClass = class_create(THIS_MODULE, CLASS_NAME);
    if(IS_ERR(prucamClass)){                // Check for error and clean up if there is
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(prucamClass);          // Correct way to return an error on a pointer
    }
    printk(KERN_INFO "prucam: device class registered correctly\n");

    // Register the device driver
    prucamDevice = device_create(prucamClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);

    // add test sysfs attr
    r = device_create_file(prucamDevice, &dev_attr_context);
    if (r < 0)
        printk(KERN_INFO "failed to create write /sys endpoint - continuing without\n");
    r = device_create_file(prucamDevice, &dev_attr_x_size);
    r = device_create_file(prucamDevice, &dev_attr_y_size);
    r = device_create_file(prucamDevice, &dev_attr_coarse_time);
    r = device_create_file(prucamDevice, &dev_attr_fine_time);
    r = device_create_file(prucamDevice, &dev_attr_y_odd_inc);
    r = device_create_file(prucamDevice, &dev_attr_green1_gain);
    r = device_create_file(prucamDevice, &dev_attr_blue_gain);
    r = device_create_file(prucamDevice, &dev_attr_red_gain);
    r = device_create_file(prucamDevice, &dev_attr_green2_gain);
    r = device_create_file(prucamDevice, &dev_attr_global_gain);
    r = device_create_file(prucamDevice, &dev_attr_analog_gain);
    r = device_create_file(prucamDevice, &dev_attr_frame_len_lines);
    r = device_create_file(prucamDevice, &dev_attr_digital_binning);
    if (r < 0)
        printk(KERN_INFO "failed to create write /sys endpoint - continuing without\n");

    //set interrupt to not triggered yet
    int_triggered = 0;

    if(IS_ERR(prucamDevice)) { // Clean up if there is an error
        class_destroy(prucamClass); // Repeated code but the alternative is goto statements
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(prucamDevice);
    }
    printk(KERN_INFO "prucam: device class created correctly\n"); // Made it! device was initialized

    /**
     * Register the platform driver. This causes the system the scan the platform
     * bus for devices that match this driver. As defined in the device-tree,
     * there should be a platform device that is found, at which point the
     * drivers probe function is called on the device and the driver can read and
     * request the interrupt line associated with that device
     */
    regDrvr = platform_driver_register(&prudrvr);
    printk(KERN_INFO "prucam: platform driver register returned: %d\n", regDrvr);

    if((r = init_cam_i2c(ar013x_i2c_info)) < 0)
        printk("i2c init failed\n");

    // init the camera control GPIO
    if((r = init_cam_gpio())) 
        return r;

    camera_enable();

    // AR0130 datasheet says sleep for a little bit after enabled vregs and clock
    msleep(10);

    // detect camera
    if((r = read_cam_reg(0x3000, &cam_ver)) < 0)
        printk("i2c read camera version failed\n");

    if(cam_ver == 0x2402) {
        printk(KERN_INFO "AR0130 detected");
        startupRegs = ar0130_startupRegs;
    }
    else if(cam_ver == 0x2406) {
        printk(KERN_INFO "AR0134 detected");
        startupRegs = ar0134_startupRegs;
    }
    else {
        printk(KERN_ERR "Uknown camera value: 0x%x", cam_ver);
    }

    // init camera i2c regs
    r = init_camera_regs(startupRegs);
    if(r < 0)
        printk(KERN_ERR "init regs using i2c failed\n");

    printk(KERN_INFO "Init Complete\n");
    return 0;
}


static void __exit prucam_exit(void) {
    int r;

    // sysfs attr
    device_remove_file(prucamDevice, &dev_attr_context);
    device_remove_file(prucamDevice, &dev_attr_x_size);
    device_remove_file(prucamDevice, &dev_attr_y_size);
    device_remove_file(prucamDevice, &dev_attr_coarse_time);
    device_remove_file(prucamDevice, &dev_attr_fine_time);
    device_remove_file(prucamDevice, &dev_attr_y_odd_inc);
    device_remove_file(prucamDevice, &dev_attr_green1_gain);
    device_remove_file(prucamDevice, &dev_attr_blue_gain);
    device_remove_file(prucamDevice, &dev_attr_red_gain);
    device_remove_file(prucamDevice, &dev_attr_green2_gain);
    device_remove_file(prucamDevice, &dev_attr_global_gain);
    device_remove_file(prucamDevice, &dev_attr_analog_gain);
    device_remove_file(prucamDevice, &dev_attr_frame_len_lines);
    device_remove_file(prucamDevice, &dev_attr_digital_binning);

    //unregister platform driver
    platform_driver_unregister(&prudrvr);

    device_destroy(prucamClass, MKDEV(majorNumber, 0));     // remove the device
    class_destroy(prucamClass);                             // remove the device class
    unregister_chrdev(majorNumber, DEVICE_NAME);            // unregister the major number

    r = end_cam_i2c();
    if(r<0)
        printk("i2c end failed\n");

    // put camera GPIO in good state and free the lines
    free_cam_gpio();

    printk(KERN_INFO "prucam: module exit\n");
}


static void free_irqs(void){
    for(int i = 0 ; i < NUM_IRQS ; i++)
        if(irqs[i].num > -1)
          free_irq(irqs[i].num, NULL);
}


static int dev_open(struct inode *inodep, struct file *filep){
    return 0;
}


/** 
 * Writes directly to the the PRU SRSR0 register that manually
 * triggers PRU system events, which can fire the PRU interrupt bit in R31.
 * Then it writes the address of the physical memory it allocated to a known
 * location in PRU shared RAM. THe PRU reads this and writes the image to this
 * address. I am writing without permission to the PRU shared RAM. This
 * should be ok, but in the future I should set aside of piece of PRU shared
 * RAM to ensure it doesn't accidentally use it
 */
static int pru_handshake(int physAddr) {

    //ioremap physical locations in the PRU shared ram 
    void __iomem *pru_shared_ram;
    pru_shared_ram = ioremap_nocache((int)PRUSHAREDRAM, 4); 

    //write physical address to PRU shared RAM where a PRU can find it
    writel(physAddr, pru_shared_ram);

    //ioremap PRU SRSR0 reg
    void __iomem *pru_srsr0;
    pru_srsr0 = ioremap_nocache((int)(PRUBASE + PRUINTC_OFFSET + SRSR0_OFFSET), 4); 

    //set bit 24 in PRU SRSR0 to trigger event 24
    writel(0x1000000, pru_srsr0);

    //TODO add a response via an interrupt from the PRU and return error

    //unmap iomem
    iounmap(pru_shared_ram);
    iounmap(pru_srsr0);

    return 0; 
}


static ssize_t dev_read(
        struct file *filep,
        char *buffer,
        size_t len,
        loff_t *offset) {

    int handshake;
    char* physBase;
    int err = 0;

    //TODO address to known location with checksum to other location this can replace the handshake
    
    mutex_lock(&cam_mtx);

    //signal PRU and tell it where to write the data
    handshake = pru_handshake((int)physAddr);
    if(handshake < 0)  {
        printk(KERN_ERR "PRU Handshake failed: %x\n", (int)physAddr);
        mutex_unlock(&cam_mtx);
        return -1;
    }

    //wait for intc to be triggered
    for(volatile int i = 0; i < (1<<27) && !int_triggered; i++);

    if(!int_triggered) {
        printk(KERN_ERR "Interrupt never triggered!\n");
        mutex_unlock(&cam_mtx);
        return -1;
    }

    int_triggered = 0;
    printk(KERN_INFO "prucam: Interrupt Triggered!\n");

    physBase = (char*)cpu_addr;

    err = copy_to_user(buffer, physBase, PIXELS); //TODO use __copy_to_user
    if(err != 0) {
        mutex_unlock(&cam_mtx);
        return -EFAULT;
    }

    mutex_unlock(&cam_mtx);

    return 0;
}


static irq_handler_t irqhandler(
        unsigned int irqN,
        void *dev_id,
        struct pt_regs *regs) {

    printk(KERN_INFO "prucam: IRQ_HANDLER: %d\n", irqN); //TODO get rid of this

    //signal that interrupt has been triggered
    int_triggered = 1;

    return (irq_handler_t) IRQ_HANDLED;
}


/**
 * @brief The device release function that is called whenever the device is closed/released by
 * the userspace program
 * @param inodep A pointer to an inode object (defined in linux/fs.h)
 * @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
    printk(KERN_INFO "prucam: Device successfully closed\n");
    return 0;
}


static uint16_t get_reg(const char *name) {
    uint16_t reg = 0;

    if(strcmp(name, "context") == 0)
        reg = 0x30B0;
    else if (strcmp(name, "x_size") == 0)
        reg = context == CONTEXT_A ? 0x3008 : 0x308E;
    else if (strcmp(name, "y_size") == 0)
        reg = context == CONTEXT_A ? 0x3006 : 0x3090;
    else if (strcmp(name, "coarse_time") == 0)
        reg = context == CONTEXT_A ? 0x3012 : 0x3016;
    else if (strcmp(name, "fine_time") == 0)
        reg = context == CONTEXT_A ? 0x3014 : 0x3018;
    else if (strcmp(name, "y_odd_inc") == 0)
        reg = context == CONTEXT_A ? 0x30A6 : 0x30A8;
    else if (strcmp(name, "green1_gain") == 0)
        reg = context == CONTEXT_A ? 0x3056 : 0x30BC;
    else if (strcmp(name, "blue_gain") == 0)
        reg = context == CONTEXT_A ? 0x3058 : 0x30BE;
    else if (strcmp(name, "red_gain") == 0)
        reg = context == CONTEXT_A ? 0x305A : 0x30C0;
    else if (strcmp(name, "green2_gain") == 0)
        reg = context == CONTEXT_A ? 0x305C : 0x30C2;
    else if (strcmp(name, "global_gain") == 0)
        reg = context == CONTEXT_A ? 0x305E : 0x30C4;
    else if (strcmp(name, "analog_gain") == 0)
        reg = 0x30B0;
    else if (strcmp(name, "frame_len_lines") == 0)
        reg = context == CONTEXT_A ? 0x300A : 0x30AA;
    else if (strcmp(name, "digital_binning") == 0)
        reg = 0x3032;
    else
        printk(KERN_ERR "Unkown store attr name: %s", name);

    return reg;
}


module_init(prucam_init);
module_exit(prucam_exit);

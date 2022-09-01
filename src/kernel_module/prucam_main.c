#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>

#include "ar0130_ctrl_regs.h"
#include "ar0134_ctrl_regs.h"
#include "ar013x_regs.h"
#include "ar013x_sysfs.h"
#include "cam_gpio.h"
#include "cam_i2c.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oliver Rew");
MODULE_DESCRIPTION("Interface to a PRU and a camera");
MODULE_VERSION("0.3.3");

#define ROWS           960
#define COLS           1280
#define PIXELS         (ROWS * COLS)
#define PRUBASE        0x4a300000
#define PRUSHAREDRAM   (PRUBASE + 0x10000)
#define PRUINTC_OFFSET 0x20000
#define SRSR0_OFFSET   0x200
#define NUM_IRQS       8

// private data
struct miscdevice miscdev;
struct mutex mutex;
dma_addr_t dma_handle = (dma_addr_t)NULL;
int *cpu_addr = NULL;
int *phys_addr = NULL;
volatile int int_triggered = 0;

typedef struct {
    char *name;
    int num;
} irq_info;

/**
 * mapping of an irq name(from the platfor device in the device device-tree)
 * to it's assigned linux irq number(in /proc/interrupt). When the irq is
 * successfully retrieved and requested, the 'num' will be assigned.
 */
irq_info irqs[] = {
    // clang-format off
    {"20", -1},
    {"21", -1},
    {"22", -1},
    {"23", -1},
    {"24", -1},
    {"25", -1},
    {"26", -1},
    {"27", -1},
    // clang-format on
};

static void free_irqs(void)
{
    for (int i = 0; i < NUM_IRQS; i++)
        if (irqs[i].num > -1)
            free_irq(irqs[i].num, NULL);
}

static int dev_open(struct inode *inodep, struct file *filep)
{
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
static int pru_handshake(int phys_addr)
{

    // ioremap physical locations in the PRU shared ram
    void __iomem *pru_shared_ram;
    pru_shared_ram = ioremap((int)PRUSHAREDRAM, 4);

    // write physical address to PRU shared RAM where a PRU can find it
    writel(phys_addr, pru_shared_ram);

    // ioremap PRU SRSR0 reg
    void __iomem *pru_srsr0;
    pru_srsr0 = ioremap((int)(PRUBASE + PRUINTC_OFFSET + SRSR0_OFFSET), 4);

    // set bit 24 in PRU SRSR0 to trigger event 24
    writel(0x1000000, pru_srsr0);

    // TODO add a response via an interrupt from the PRU and return error

    // unmap iomem
    iounmap(pru_shared_ram);
    iounmap(pru_srsr0);

    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
                        loff_t *offset)
{

    int handshake;
    char *phys_base;
    int ret;

    int_triggered = 0; // init int_triggered to false on read start

    // TODO address to known location with checksum to other location this can
    // replace the handshake

    mutex_lock(&mutex);

    // signal PRU and tell it where to write the data
    printk(KERN_INFO "prucam: sending handshake.");
    handshake = pru_handshake((int)phys_addr);
    if (handshake < 0) {
        printk(KERN_ERR "prucam: PRU Handshake failed: %p\n", phys_addr);
        mutex_unlock(&mutex);
        return -1;
    }

    // wait for intc to be triggered
    for (volatile int i = 0; i < (1 << 27) && !int_triggered; i++);

    if (!int_triggered) {
        printk(KERN_ERR "prucam: Interrupt never triggered!\n");
        mutex_unlock(&mutex);
        return -1;
    }

    int_triggered = 0;
    printk(KERN_INFO "prucam: Interrupt Triggered!\n");

    phys_base = (char *)cpu_addr;

    ret = copy_to_user(buffer, phys_base, PIXELS); // TODO use __copy_to_user
    if (ret) {
        printk(KERN_ERR "prucam: copy to user failed\n");
        mutex_unlock(&mutex);
        return -EFAULT;
    }

    mutex_unlock(&mutex);

    return 0;
}

static irq_handler_t irqhandler(unsigned int irq_num, void *dev_id,
                                struct pt_regs *regs)
{

    // signal that interrupt has been triggered
    int_triggered = 1;

    printk(KERN_INFO "prucam: irq %d handled.\n", irq_num);

    return (irq_handler_t)IRQ_HANDLED;
}

/**
 * @brief The device release function that is called whenever the device is
 * closed/released by the userspace program
 * @param inodep A pointer to an inode object (defined in linux/fs.h)
 * @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "prucam: Device successfully closed\n");
    return 0;
}

/**
 * TODO - dedicated error interrupt line from PRU that triggers handler to shut
 * down other opperations
 */
static const struct file_operations prucam_fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .read    = dev_read,
    .release = dev_release,
};

static int prucam_probe(struct platform_device *pdev)
{
    struct device *dev;
    struct device_node *node = pdev->dev.of_node;
    camera_regs_t *startup_regs = NULL;
    int ret, irq;
    u16 cam_ver;

    if (!node)
        return -ENODEV; /* No support for non-DT platforms */

    /* Get a handle to the PRUSS structures */
    dev = &pdev->dev;

    /* Get interrupts and install interrupt handlers */
    for (int i = 0; i < NUM_IRQS; i++) {
        irq = platform_get_irq_byname(pdev, irqs[i].name);
        if (irq < 0) {
            ret = irq;
            dev_err(dev, "Unable to get irq %s: %d\n", irqs[i].name, ret);
            goto error_get_irq;
        }

        /* Save irq numbers for freeing */
        irqs[i].num = irq;

        ret = request_irq(irq, (irq_handler_t)irqhandler, IRQF_TRIGGER_RISING,
                          dev_name(dev), NULL);
        if (ret < 0) {
            dev_err(dev, "Unable to request irq %s: %d\n", irqs[i].name, ret);
            goto error_request_irq;
        }
    }

    // set DMA mask
    ret = dma_set_coherent_mask(dev, 0xffffffff);
    if (ret) {
        dev_err(dev, "Failed to set DMA mask : error %d\n", ret);
        goto error_dma_set;
    }

     // I initial used GFP_DMA flag below, but I could not allocate >1 MiB
     // I am unsure what the ideal flags for this are, but GFP_KERNEL seems to
     // work
    cpu_addr = dma_alloc_coherent(dev, PIXELS, &dma_handle, GFP_KERNEL);
    if (!cpu_addr) {
        dev_err(dev, "Failed to allocate DMA\n");
        ret = -1;
        goto error_dma_alloc;
    }

    printk(KERN_INFO "prucam: virtual address: %p\n", cpu_addr);

    phys_addr = (int *)dma_handle;
    printk(KERN_INFO "prucam: physical dddress: %p\n", phys_addr);
    int_triggered = 0;

    ret = init_cam_i2c();
    if (ret < 0) {
        dev_err(dev, "Init camera i2c failed: %d.\n", ret);
        goto error_i2c;
    }

    // init the camera control GPIO
    ret = init_cam_gpio();
    if (ret < 0) {
        dev_err(dev, "Init camrea gpio failed: %d.\n", ret);
        goto error_gpio;
    }

    camera_enable();

    // detect camera
    ret = read_cam_reg(AR013X_AD_CHIP_VERSION_REG, &cam_ver);
    if (ret < 0) {
        dev_err(dev, "Read camera version over i2c failed\n");
        goto error_i2c_rw;
    }

    if (cam_ver == 0x2402) {
        printk(KERN_INFO "prucam: AR0130 detected");
        startup_regs = ar0130_startup_regs;
    } else if (cam_ver == 0x2406) {
        printk(KERN_INFO "prucam: AR0134 detected");
        startup_regs = ar0134_startup_regs;
    } else {
        dev_err(dev, "Uknown camera value: 0x%x", cam_ver);
        goto error_i2c_rw;
    }

    // init camera i2c regs
    ret = init_camera_regs(startup_regs);
    if (ret < 0) {
        dev_err(dev, "init regs using i2c failed\n");
        goto error_i2c_rw;
    }

    // add sysfs
    ret = sysfs_create_groups(&dev->kobj, ar013x_groups);
    if (ret) {
        dev_err(dev, "Registration failed.\n");
        goto error_sysfs;
    }

    // add misc device for file ops
    miscdev.fops = &prucam_fops;
    miscdev.minor = MISC_DYNAMIC_MINOR;
    miscdev.mode = S_IRUGO;
    miscdev.name = "prucam";
    ret = misc_register(&miscdev);
    if (ret)
        goto error_misc;

    mutex_init(&mutex);

    printk("prucam probe complete");
    return 0;

error_misc:
    sysfs_remove_groups(&dev->kobj, ar013x_groups);
error_sysfs:
error_i2c_rw:
    free_cam_gpio();
error_gpio:
    end_cam_i2c();
error_i2c:
    dma_free_coherent(dev, PIXELS, cpu_addr, dma_handle);
error_dma_alloc:
error_dma_set:
error_get_irq:
    free_irqs();
error_request_irq:
    printk("prucam probe failed with: %d\n", ret);
    return ret;
}

static int prucam_remove(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    
    misc_deregister(&miscdev);

    // remove the sysfs attr
    sysfs_remove_groups(&dev->kobj, ar013x_groups);

    // put camera GPIO in good state and free the lines
    free_cam_gpio();

    end_cam_i2c();

    dma_free_coherent(dev, PIXELS, cpu_addr, dma_handle);

    free_irqs();

    printk("prucam removed\n");
    return 0;
}

static const struct of_device_id prucam_of_ids[] = {
    { .compatible = "prudev,prudev"},
    { /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, prucam_of_ids);

static struct platform_driver prucam_driver = {
    .driver = {
        .name = "prudev",
        .owner = THIS_MODULE,
        .of_match_table = prucam_of_ids,
    },
    .probe = prucam_probe,
    .remove = prucam_remove,
};
module_platform_driver(prucam_driver);

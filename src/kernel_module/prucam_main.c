#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pruss.h>
#include <linux/remoteproc.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>
#include <linux/completion.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oliver Rew");
MODULE_AUTHOR("Ryan Medick");
MODULE_DESCRIPTION("AM335x PRU Camera Interface Driver");
MODULE_VERSION("1.0.0");

#define ROWS           1024
#define COLS           1280 * 2 // 2 bytes / pixel
#define FRAME_BYTES    (ROWS * COLS)
#define PRU0_FW_NAME   "prucam_pru0_fw.out"
#define PRU1_FW_NAME   "prucam_pru1_fw.out"

// private data
struct miscdevice miscdev;
struct mutex mutex;

/**
 * physical/virtual addresses of the frame buffer used to transfer image from 
 * PRU to kernel
 */
dma_addr_t frame_buffer_pa = (dma_addr_t)NULL;
int *frame_buffer_va = NULL;

/**
 * completion to signal interrupt was received from PRU, signalling that the
 * image was captured and copied to the buffer
 */
static DECLARE_COMPLETION(pru_to_arm_irq_trigger);

struct rproc *pru0 = NULL;
struct rproc *pru1 = NULL;

/* underlying pruss object */
struct pruss *pruss; 

/* PRU shared memory region */
struct pruss_mem_region shared_mem;

static irqreturn_t pru_irq_handler(int irq_num, void *);

typedef struct {
    char *name;
    int num;
    irq_handler_t handler;
} irq_info_t;

/**
 * interrupts as defined in the device-tree overlay. The number is the value
 * returned by platform_get_irq_byname. The last 2 are inter-PRU interrupts and
 * don't interrupt the kernel. However, we still use the kernel driver to
 * configure them, but specifiy the 'no_action' handler
 */
irq_info_t irqs[] = {
    {.name = "pru1_to_arm", .num = -1, .handler = pru_irq_handler},
    {.name = "arm_to_prus", .num = -1, .handler = no_action},
    {.name = "pru0_to_pru1", .num = -1, .handler = no_action},
};

static void free_irqs(void)
{
    for (int i = 0; i < (sizeof(irqs) / sizeof(irqs[0])); i++)
        if (irqs[i].num > -1)
            free_irq(irqs[i].num, NULL);
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
                        loff_t *offset)
{
    int ret;

    mutex_lock(&mutex);

    printk(KERN_INFO "prucam: signalling PRUs to capture image.");

    /* Trigger ARM to PRUs interrupt to start image capture */
    irq_set_irqchip_state(irqs[1].num, IRQCHIP_STATE_PENDING, true);

    /* Wait for intc to be triggered for 500ms */
    ret = wait_for_completion_timeout(&pru_to_arm_irq_trigger, msecs_to_jiffies(500));
    if (ret == 0) {
        printk(KERN_ERR "prucam: interrupt never triggered\n");
        mutex_unlock(&mutex);
        return -1;
    }

    printk(KERN_INFO "prucam: image captured\n");

    /* copy the image to the caller */
    ret = copy_to_user(buffer, (char*)frame_buffer_va, FRAME_BYTES);
    if (ret) {
        printk(KERN_ERR "prucam: copy to user failed\n");
        mutex_unlock(&mutex);
        return -EFAULT;
    }

    mutex_unlock(&mutex);

    return FRAME_BYTES;
}

static irqreturn_t pru_irq_handler(int irq_num, void *dev_id)
{
    /* Signal that interrupt has been triggered */
    complete(&pru_to_arm_irq_trigger);

    return IRQ_HANDLED;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    return 0;
}

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
    int ret, irq;

    if (!node)
        return -ENODEV; /* No support for non-DT platforms */

    /* Get a handle to the PRUSS structures */
    dev = &pdev->dev;

    dev_info(dev, "probing device: %s\n", pdev->name);

    /* Get PRUs */
    pru0 = pru_rproc_get(node, PRUSS_PRU0, NULL);
    if (IS_ERR(pru0)) {
        ret = PTR_ERR(pru0);
        if (ret != -EPROBE_DEFER)
            dev_err(dev, "Unable to get PRU0.\n");
        goto error_get_pru0;
    }
    pru1 = pru_rproc_get(node, PRUSS_PRU1, NULL);
    if (IS_ERR(pru1)) {
        ret = PTR_ERR(pru1);
        if (ret != -EPROBE_DEFER)
            dev_err(dev, "Unable to get PRU1.\n");
        goto error_get_pru1;
    }

    /* Get the underlying PRUSS object */
    pruss = pruss_get(pru0);
    if (IS_ERR(pruss)) {
        dev_err(dev, "error requesting shared memory region: %d", ret);
        ret = PTR_ERR(pruss);
        goto err_pruss_get;
    }

    /* Request the shared memory region */
    ret = pruss_request_mem_region(pruss, PRUSS_MEM_SHRD_RAM2, &shared_mem);
    if (ret) {
        dev_err(dev, "error requesting shared memory region: %d", ret);
        goto err_shared_mem;
    }

    /* Get interrupts and install interrupt handlers */
    for (int i = 0; i < (sizeof(irqs) / sizeof(irqs[0])); i++) {
        /* Get the irq based on the name in the device tree node */
        irq = platform_get_irq_byname(pdev, irqs[i].name);
        if (irq < 0) {
            ret = irq;
            dev_err(dev, "Unable to get irq %s: %d\n", irqs[i].name, ret);
            goto error_irq;
        }

        dev_info(dev, "irq: %s -> %d\n", irqs[i].name, irq);
        
        /* Request that irq from the kernel */
        ret = request_irq(irq, irqs[i].handler, IRQ_TYPE_LEVEL_HIGH, 
                          dev_name(dev), NULL);
        if (ret < 0) {
            dev_err(dev, "Unable to request irq %s: %d\n", irqs[i].name, ret);
            goto error_irq;
        }

        /* Save irq numbers for freeing */
        irqs[i].num = irq;
    }

    /* Set firmware for PRUs */
    ret = rproc_set_firmware(pru0, PRU0_FW_NAME);
    if (ret) {
        dev_err(dev, "Failed to set PRU0 firmware %s: %d\n",
            PRU0_FW_NAME, ret);
        goto error_set_pru0_fw;
    }
    ret = rproc_set_firmware(pru1, PRU1_FW_NAME);
    if (ret) {
        dev_err(dev, "Failed to set PRU1 firmware %s: %d\n",
            PRU1_FW_NAME, ret);
        goto error_set_pru1_fw;
    }

    /* Boot PRUs (boot PRU1 1st) */
    ret = rproc_boot(pru1);
    if (ret) {
        dev_err(dev, "Failed to boot PRU1: %d\n", ret);
        goto error_boot_pru1;
    }
    ret = rproc_boot(pru0);
    if (ret) {
        dev_err(dev, "Failed to boot PRU0: %d\n", ret);
        goto error_boot_pru0;
    }

    /* Set DMA mask */
    ret = dma_set_coherent_mask(dev, 0xffffffff);
    if (ret) {
        dev_err(dev, "Failed to set DMA mask : error %d\n", ret);
        goto error_dma_set;
    }

    /* Allocate a physically contiguous frame buffer */
    frame_buffer_va = dma_alloc_coherent(dev, FRAME_BYTES, &frame_buffer_pa, GFP_KERNEL);
    if (!frame_buffer_va) {
        dev_err(dev, "Failed to allocate DMA\n");
        ret = -1;
        goto error_dma_alloc;
    }

    dev_info(dev, "prucam: frame buffer virt/phys: 0x%p/0x%p\n", frame_buffer_va, (void*)frame_buffer_pa);

    /**
     * Write the frame buffer physical address to the base of PRU shared mem. 
     * The PRUs will read the address from here and then write the image to
     * this buffer. In the future, this will like be a struct with additional
     * information to be transferred to the PRUs. 
     * TODO We are technically writing to this memory without the PRUs permission 
     * and it would be preferrable to somehow allocate this memory in the PRUs, 
     * perhaps with the linker script, so it could not be clobbered
     * TODO write checksum to other location for PRU to verify
     */
    writel((int)frame_buffer_pa, shared_mem.va);

    mutex_init(&mutex);

    printk("prucam probe complete");
    return 0;

error_dma_alloc:
error_dma_set:
    rproc_shutdown(pru0);
error_boot_pru0:
    rproc_shutdown(pru1);
error_boot_pru1:
error_set_pru1_fw:
error_set_pru0_fw:
error_irq:
    free_irqs();
    pruss_release_mem_region(pruss, &shared_mem);
err_shared_mem:
    pruss_put(pruss);
err_pruss_get:
    pru_rproc_put(pru1);
error_get_pru1:
    pru_rproc_put(pru0);
error_get_pru0:
    printk("prucam probe failed with: %d\n", ret);
    return ret;
}

static int prucam_remove(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    
    dma_free_coherent(dev, FRAME_BYTES, frame_buffer_va, frame_buffer_pa);

    /* Free the shared mem region and pruss */
    pruss_release_mem_region(pruss, &shared_mem);
    pruss_put(pruss);

    /* Stop PRUs */
    rproc_shutdown(pru0);
    rproc_shutdown(pru1);

    free_irqs();

    pru_rproc_put(pru1);
    pru_rproc_put(pru0);

    printk("prucam removed\n");
    return 0;
}

static const struct of_device_id prucam_of_ids[] = {
    { .compatible = "prucam,prucam"},
    { /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, prucam_of_ids);

static struct platform_driver prucam_driver = {
    .driver = {
        .name = "prucam",
        .owner = THIS_MODULE,
        .of_match_table = prucam_of_ids,
    },
    .probe = prucam_probe,
    .remove = prucam_remove,
};

module_platform_driver(prucam_driver);

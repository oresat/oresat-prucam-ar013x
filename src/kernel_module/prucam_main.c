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
#include <asm/io.h>

#include "ar0130_ctrl_regs.h"
#include "ar0134_ctrl_regs.h"
#include "ar013x_regs.h"
#include "ar013x_sysfs.h"
#include "cam_gpio.h"
#include "cam_i2c.h"

#include <linux/videodev2.h>
#include <linux/v4l2-dv-timings.h>
#include <media/v4l2-device.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-dv-timings.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include <media/videobuf2-v4l2.h>
#include <media/videobuf2-dma-contig.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oliver Rew");
MODULE_AUTHOR("Ryan Medick");
MODULE_DESCRIPTION("AM335x PRU Camera Interface Driver");
MODULE_VERSION("1.0.0");

#define ROWS           960
#define COLS           1280
#define PIXELS         (ROWS * COLS)
#define PRU0_FW_NAME   "prucam_pru0_fw.out"
#define PRU1_FW_NAME   "prucam_pru1_fw.out"

#define SKEL_TVNORMS V4L2_STD_ALL

// private data
struct miscdevice miscdev;
struct mutex cam_mutex;

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

    mutex_lock(&cam_mutex);

    printk(KERN_INFO "prucam: signalling PRUs to capture image.");

    /* Trigger ARM to PRUs interrupt to start image capture */
    irq_set_irqchip_state(irqs[1].num, IRQCHIP_STATE_PENDING, true);

    /* Wait for intc to be triggered for 500ms */
    ret = wait_for_completion_timeout(&pru_to_arm_irq_trigger, msecs_to_jiffies(500));
    if (ret == 0) {
        printk(KERN_ERR "prucam: interrupt never triggered\n");
        mutex_unlock(&cam_mutex);
        return -1;
    }

    printk(KERN_INFO "prucam: image captured\n");

    /* copy the image to the caller */
    ret = copy_to_user(buffer, (char*)frame_buffer_va, PIXELS);
    if (ret) {
        printk(KERN_ERR "prucam: copy to user failed\n");
        mutex_unlock(&cam_mutex);
        return -EFAULT;
    }

    mutex_unlock(&cam_mutex);

    return PIXELS;
}

static irqreturn_t pru_irq_handler(int irq_num, void *dev_id)
{
    /* Signal that interrupt has been triggered */

    //printk(KERN_INFO "FRAME");
    /* Trigger ARM to PRUs interrupt to start image capture */
    //irq_set_irqchip_state(irqs[1].num, IRQCHIP_STATE_PENDING, true);
    // TODO fix me
    irq_set_irqchip_state(irqs[1].num, IRQCHIP_STATE_PENDING, true);
    //complete(&pru_to_arm_irq_trigger);


    /*
     * TODO OLIVER: add logic here. grab buffer from head of list and write
     * phys addr to PRU
    // TODO use list_for_each for something?
    int i;
    for (i = 0; i < skel->queue.num_buffers; i++) {
        struct vb2_buffer* vb = skel->queue.bufs[i];

        dma_addr_t mem = vb2_dma_contig_plane_dma_addr(vb, 0);
        printk(KERN_INFO "BUFX[%d] %s: 0x%p\n", i, vb2_state_name(vb->state), (void*)mem);
    }


    //printk(KERN_INFO "BUF STATE: %s\n", vb2_state_name(vbuf->vb2_buf.state));
    // TODO TEMP REQUEUE BUFF
    list_del(&buf->list);
    vb2_buffer_done(vb, VB2_BUF_STATE_DONE);
    */

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

// ========================
struct skeleton {
    struct platform_device *pdev;
    struct v4l2_device v4l2_dev;
    struct video_device vdev;
    struct v4l2_ctrl_handler ctrl_handler;
    struct mutex lock;
    v4l2_std_id std;
    struct v4l2_dv_timings timings;
    struct v4l2_pix_format format;
    unsigned input;

    struct vb2_queue queue;

    spinlock_t qlock;
    struct list_head buf_list;
    unsigned field;
    unsigned sequence;
};

// TODO store this in driver data
struct skeleton *skel;

struct skel_buffer {
    struct vb2_v4l2_buffer vb;
    struct list_head list;
};

static inline struct skel_buffer *to_skel_buffer(struct vb2_v4l2_buffer *vbuf)
{
    return container_of(vbuf, struct skel_buffer, vb);
}

static const struct v4l2_dv_timings_cap skel_timings_cap = {
    .type = V4L2_DV_BT_656_1120,
    /* keep this initialization for compatibility with GCC < 4.4.6 */
    .reserved = { 0 },
    V4L2_INIT_BT_TIMINGS(
            COLS, COLS,		/* min/max width */
            ROWS, ROWS,		/* min/max height */
            27000000, 74250000,	/* min/max pixelclock*/
            V4L2_DV_BT_STD_CEA861,	/* Supported standards */
            /* capabilities */
            V4L2_DV_BT_CAP_INTERLACED | V4L2_DV_BT_CAP_PROGRESSIVE
            )
};

static int skeleton_s_ctrl(struct v4l2_ctrl *ctrl)
{
    /*struct skeleton *skel =
      container_of(ctrl->handler, struct skeleton, ctrl_handler);*/

    switch (ctrl->id) {
        case V4L2_CID_BRIGHTNESS:
            /* TODO: set brightness to ctrl->val */
            break;
        case V4L2_CID_CONTRAST:
            /* TODO: set contrast to ctrl->val */
            break;
        case V4L2_CID_SATURATION:
            /* TODO: set saturation to ctrl->val */
            break;
        case V4L2_CID_HUE:
            /* TODO: set hue to ctrl->val */
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static const struct v4l2_ctrl_ops skel_ctrl_ops = {
    .s_ctrl = skeleton_s_ctrl,
};

static void skeleton_fill_pix_format(struct skeleton *skel,
        struct v4l2_pix_format *pix)
{
    pix->pixelformat = V4L2_PIX_FMT_YUYV;
    if (skel->input == 0) {
        printk(KERN_INFO "S VIDEO SET");
        /* S-Video input */
        //pix->width = 720;
        //pix->height = (skel->std & V4L2_STD_525_60) ? 480 : 576;
        pix->width = COLS;
        pix->height = ROWS;
        pix->field = V4L2_FIELD_INTERLACED;
        pix->colorspace = V4L2_COLORSPACE_SMPTE170M;
    } else {
        /* HDMI input */
        printk(KERN_INFO "HDMI INPUT SET");
        pix->width = skel->timings.bt.width;
        pix->height = skel->timings.bt.height;
        if (skel->timings.bt.interlaced) {
            pix->field = V4L2_FIELD_ALTERNATE;
            pix->height /= 2;
        } else {
            pix->field = V4L2_FIELD_NONE;
        }
        pix->colorspace = V4L2_COLORSPACE_REC709;
    }

    /*
     * The YUYV format is four bytes for every two pixels, so bytesperline
     * is width * 2.
     */
    pix->bytesperline = pix->width * 2;
    pix->sizeimage = pix->bytesperline * pix->height;
    pix->priv = 0;
}

/*
 * Setup the constraints of the queue: besides setting the number of planes
 * per buffer and the size and allocation context of each plane, it also
 * checks if sufficient buffers have been allocated. Usually 3 is a good
 * minimum number: many DMA engines need a minimum of 2 buffers in the
 * queue and you need to have another available for userspace processing.
 */
static int queue_setup(struct vb2_queue *vq,
        unsigned int *nbuffers, unsigned int *nplanes,
        unsigned int sizes[], struct device *alloc_devs[])
{
    struct skeleton *skel = vb2_get_drv_priv(vq);

    skel->field = skel->format.field;
    if (skel->field == V4L2_FIELD_ALTERNATE) {
        /*
         * You cannot use read() with FIELD_ALTERNATE since the field
         * information (TOP/BOTTOM) cannot be passed back to the user.
         */
        if (vb2_fileio_is_active(vq))
            return -EINVAL;
        skel->field = V4L2_FIELD_TOP;
    }

    if (vq->num_buffers + *nbuffers < 3)
        *nbuffers = 3 - vq->num_buffers;

    if (*nplanes)
        return sizes[0] < skel->format.sizeimage ? -EINVAL : 0;
    *nplanes = 1;
    sizes[0] = skel->format.sizeimage;
    return 0;
}

/*
 * Prepare the buffer for queueing to the DMA engine: check and set the
 * payload size.
 */
static int buffer_prepare(struct vb2_buffer *vb)
{
    struct skeleton *skel = vb2_get_drv_priv(vb->vb2_queue);
    unsigned long size = skel->format.sizeimage;

    if (vb2_plane_size(vb, 0) < size) {
        dev_err(&skel->pdev->dev, "buffer too small (%lu < %lu)\n",
                vb2_plane_size(vb, 0), size);
        return -EINVAL;
    }

    vb2_set_plane_payload(vb, 0, size);
    return 0;
}

static const char *vb2_state_name(enum vb2_buffer_state s)
{
	static const char * const state_names[] = {
		[VB2_BUF_STATE_DEQUEUED] = "dequeued",
		[VB2_BUF_STATE_IN_REQUEST] = "in request",
		[VB2_BUF_STATE_PREPARING] = "preparing",
		[VB2_BUF_STATE_QUEUED] = "queued",
		[VB2_BUF_STATE_ACTIVE] = "active",
		[VB2_BUF_STATE_DONE] = "done",
		[VB2_BUF_STATE_ERROR] = "error",
	};

	if ((unsigned int)(s) < ARRAY_SIZE(state_names))
		return state_names[s];
	return "unknown";
}

/*
 * Queue this buffer to the DMA engine.
 */
static void buffer_queue(struct vb2_buffer *vb)
{
    struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
    struct skeleton *skel = vb2_get_drv_priv(vb->vb2_queue);
    struct skel_buffer *buf = to_skel_buffer(vbuf);
    unsigned long flags;

    spin_lock_irqsave(&skel->qlock, flags);
    list_add_tail(&buf->list, &skel->buf_list);
    // print the phys addr of the buffer
    dma_addr_t mem = vb2_dma_contig_plane_dma_addr(vb, 0);
    printk(KERN_INFO "RETURN BUF: %dB -> 0x%p\n", vb->planes[0].length, (void*)mem);

    /*
     * TODO if the irq handler was unable to find a buffer to give to the PRU
     * in the IRQ handler, it cannot wait and so the PRU will be doing nothing.
     * Perhaps here we check if the PRU has no buffers and if so, give it one
     */

    int i;
    for (i = 0; i < skel->queue.num_buffers; i++) {
        struct vb2_buffer* tvb = skel->queue.bufs[i];
        dma_addr_t mem = vb2_dma_contig_plane_dma_addr(tvb, 0);
        printk(KERN_INFO "BUFX[%d] %s: 0x%p\n", i, vb2_state_name(tvb->state), (void*)mem);
    }


    //printk(KERN_INFO "BUF STATE: %s\n", vb2_state_name(vbuf->vb2_buf.state));
    // TODO TEMP REQUEUE BUFF
    list_del(&buf->list);
    vb2_buffer_done(vb, VB2_BUF_STATE_DONE);


    spin_unlock_irqrestore(&skel->qlock, flags);
}

static void return_all_buffers(struct skeleton *skel,
        enum vb2_buffer_state state)
{
    struct skel_buffer *buf, *node;
    unsigned long flags;

    spin_lock_irqsave(&skel->qlock, flags);
    list_for_each_entry_safe(buf, node, &skel->buf_list, list) {
        vb2_buffer_done(&buf->vb.vb2_buf, state);
        list_del(&buf->list);
    }
    spin_unlock_irqrestore(&skel->qlock, flags);
}

/*
 * Start streaming. First check if the minimum number of buffers have been
 * queued. If not, then return -ENOBUFS and the vb2 framework will call
 * this function again the next time a buffer has been queued until enough
 * buffers are available to actually start the DMA engine.
 */
static int start_streaming(struct vb2_queue *vq, unsigned int count)
{
    struct skeleton *skel = vb2_get_drv_priv(vq);
    int ret = 0;

    skel->sequence = 0;

    /* TODO: start DMA */

    if (ret) {
        /*
         * In case of an error, return all active buffers to the
         * QUEUED state
         */
        return_all_buffers(skel, VB2_BUF_STATE_QUEUED);
    }
    return ret;
}

/*
 * Stop the DMA engine. Any remaining buffers in the DMA queue are dequeued
 * and passed on to the vb2 framework marked as STATE_ERROR.
 */
static void stop_streaming(struct vb2_queue *vq)
{
    struct skeleton *skel = vb2_get_drv_priv(vq);

    /* TODO: stop DMA */

    /* Release all active buffers */
    return_all_buffers(skel, VB2_BUF_STATE_ERROR);
}

/*
 * Required ioctl querycap. Note that the version field is prefilled with
 * the version of the kernel.
 */
static int skeleton_querycap(struct file *file, void *priv,
        struct v4l2_capability *cap)
{
    struct skeleton *skel = video_drvdata(file);

    strlcpy(cap->driver, KBUILD_MODNAME, sizeof(cap->driver));
    strlcpy(cap->card, "V4L2 PCI Skeleton", sizeof(cap->card));
    snprintf(cap->bus_info, sizeof(cap->bus_info), "NAME_HERE");
    return 0;
}

static int skeleton_try_fmt_vid_cap(struct file *file, void *priv,
        struct v4l2_format *f)
{
    /* Signal PRU to start image capture */
    struct skeleton *skel = video_drvdata(file);
    struct v4l2_pix_format *pix = &f->fmt.pix;

    printk(KERN_INFO "PIX: %x\n", pix->pixelformat);

    /*
     * Due to historical reasons providing try_fmt with an unsupported
     * pixelformat will return -EINVAL for video receivers. Webcam drivers,
     * however, will silently correct the pixelformat. Some video capture
     * applications rely on this behavior...
     */
    if (pix->pixelformat != V4L2_PIX_FMT_YUYV)
        return -EINVAL;
    skeleton_fill_pix_format(skel, pix);
    return 0;
}

static int skeleton_s_fmt_vid_cap(struct file *file, void *priv,
        struct v4l2_format *f)
{
    printk(KERN_INFO "S_FMT");
    struct skeleton *skel = video_drvdata(file);
    int ret;

    ret = skeleton_try_fmt_vid_cap(file, priv, f);
    if (ret)
        return ret;

    /*
     * It is not allowed to change the format while buffers for use with
     * streaming have already been allocated.
     */
    if (vb2_is_busy(&skel->queue))
        return -EBUSY;

    /* TODO: change format */
    skel->format = f->fmt.pix;
    return 0;
}

static int skeleton_g_fmt_vid_cap(struct file *file, void *priv,
        struct v4l2_format *f)
{
    printk(KERN_INFO "G_FMT");
    struct skeleton *skel = video_drvdata(file);

    f->fmt.pix = skel->format;
    return 0;
}

static int skeleton_enum_fmt_vid_cap(struct file *file, void *priv,
        struct v4l2_fmtdesc *f)
{
    if (f->index != 0)
        return -EINVAL;

    f->pixelformat = V4L2_PIX_FMT_YUYV;
    return 0;
}

static int skeleton_s_std(struct file *file, void *priv, v4l2_std_id std)
{
    struct skeleton *skel = video_drvdata(file);

    /* S_STD is not supported on the HDMI input */
    if (skel->input)
        return -ENODATA;

    /*
     * No change, so just return. Some applications call S_STD again after
     * the buffers for streaming have been set up, so we have to allow for
     * this behavior.
     */
    if (std == skel->std)
        return 0;

    /*
     * Changing the standard implies a format change, which is not allowed
     * while buffers for use with streaming have already been allocated.
     */
    if (vb2_is_busy(&skel->queue))
        return -EBUSY;

    /* TODO: handle changing std */

    skel->std = std;

    /* Update the internal format */
    skeleton_fill_pix_format(skel, &skel->format);
    return 0;
}

static int skeleton_g_std(struct file *file, void *priv, v4l2_std_id *std)
{
    struct skeleton *skel = video_drvdata(file);

    /* G_STD is not supported on the HDMI input */
    if (skel->input)
        return -ENODATA;

    *std = skel->std;
    return 0;
}

/*
 * Query the current standard as seen by the hardware. This function shall
 * never actually change the standard, it just detects and reports.
 * The framework will initially set *std to tvnorms (i.e. the set of
 * supported standards by this input), and this function should just AND
 * this value. If there is no signal, then *std should be set to 0.
 */
static int skeleton_querystd(struct file *file, void *priv, v4l2_std_id *std)
{
    struct skeleton *skel = video_drvdata(file);

    /* QUERY_STD is not supported on the HDMI input */
    if (skel->input)
        return -ENODATA;

#ifdef TODO
    /*
     * Query currently seen standard. Initial value of *std is
     * V4L2_STD_ALL. This function should look something like this:
     */
    get_signal_info();
    if (no_signal) {
        *std = 0;
        return 0;
    }
    /* Use signal information to reduce the number of possible standards */
    if (signal_has_525_lines)
        *std &= V4L2_STD_525_60;
    else
        *std &= V4L2_STD_625_50;
#endif
    return 0;
}

static int skeleton_s_dv_timings(struct file *file, void *_fh,
        struct v4l2_dv_timings *timings)
{
    struct skeleton *skel = video_drvdata(file);

    /* S_DV_TIMINGS is not supported on the S-Video input */
    if (skel->input == 0)
        return -ENODATA;

    /* Quick sanity check */
    if (!v4l2_valid_dv_timings(timings, &skel_timings_cap, NULL, NULL))
        return -EINVAL;

    /* Check if the timings are part of the CEA-861 timings. */
    if (!v4l2_find_dv_timings_cap(timings, &skel_timings_cap,
                0, NULL, NULL))
        return -EINVAL;

    /* Return 0 if the new timings are the same as the current timings. */
    if (v4l2_match_dv_timings(timings, &skel->timings, 0, false))
        return 0;

    /*
     * Changing the timings implies a format change, which is not allowed
     * while buffers for use with streaming have already been allocated.
     */
    if (vb2_is_busy(&skel->queue))
        return -EBUSY;

    /* TODO: Configure new timings */

    /* Save timings */
    skel->timings = *timings;

    /* Update the internal format */
    skeleton_fill_pix_format(skel, &skel->format);
    return 0;
}

static int skeleton_g_dv_timings(struct file *file, void *_fh,
        struct v4l2_dv_timings *timings)
{
    struct skeleton *skel = video_drvdata(file);

    /* G_DV_TIMINGS is not supported on the S-Video input */
    if (skel->input == 0)
        return -ENODATA;

    *timings = skel->timings;
    return 0;
}

static int skeleton_enum_dv_timings(struct file *file, void *_fh,
        struct v4l2_enum_dv_timings *timings)
{
    struct skeleton *skel = video_drvdata(file);

    /* ENUM_DV_TIMINGS is not supported on the S-Video input */
    if (skel->input == 0)
        return -ENODATA;

    return v4l2_enum_dv_timings_cap(timings, &skel_timings_cap,
            NULL, NULL);
}

/*
 * Query the current timings as seen by the hardware. This function shall
 * never actually change the timings, it just detects and reports.
 * If no signal is detected, then return -ENOLINK. If the hardware cannot
 * lock to the signal, then return -ENOLCK. If the signal is out of range
 * of the capabilities of the system (e.g., it is possible that the receiver
 * can lock but that the DMA engine it is connected to cannot handle
 * pixelclocks above a certain frequency), then -ERANGE is returned.
 */
static int skeleton_query_dv_timings(struct file *file, void *_fh,
        struct v4l2_dv_timings *timings)
{
    struct skeleton *skel = video_drvdata(file);

    /* QUERY_DV_TIMINGS is not supported on the S-Video input */
    if (skel->input == 0)
        return -ENODATA;

#ifdef TODO
    /*
     * Query currently seen timings. This function should look
     * something like this:
     */
    detect_timings();
    if (no_signal)
        return -ENOLINK;
    if (cannot_lock_to_signal)
        return -ENOLCK;
    if (signal_out_of_range_of_capabilities)
        return -ERANGE;

    /* Useful for debugging */
    v4l2_print_dv_timings(skel->v4l2_dev.name, "query_dv_timings:",
            timings, true);
#endif
    return 0;
}

static int skeleton_dv_timings_cap(struct file *file, void *fh,
        struct v4l2_dv_timings_cap *cap)
{
    struct skeleton *skel = video_drvdata(file);

    /* DV_TIMINGS_CAP is not supported on the S-Video input */
    if (skel->input == 0)
        return -ENODATA;
    *cap = skel_timings_cap;
    return 0;
}

static int skeleton_enum_input(struct file *file, void *priv,
        struct v4l2_input *i)
{
    if (i->index > 1)
        return -EINVAL;

    i->type = V4L2_INPUT_TYPE_CAMERA;
    if (i->index == 0) {
        i->std = SKEL_TVNORMS;
        strlcpy(i->name, "S-Video", sizeof(i->name));
        i->capabilities = V4L2_IN_CAP_STD;
    } else {
        i->std = 0;
        strlcpy(i->name, "HDMI", sizeof(i->name));
        i->capabilities = V4L2_IN_CAP_DV_TIMINGS;
    }
    return 0;
}

static int skeleton_s_input(struct file *file, void *priv, unsigned int i)
{
    struct skeleton *skel = video_drvdata(file);

    if (i > 1)
        return -EINVAL;

    /*
     * Changing the input implies a format change, which is not allowed
     * while buffers for use with streaming have already been allocated.
     */
    if (vb2_is_busy(&skel->queue))
        return -EBUSY;

    skel->input = i;
    /*
     * Update tvnorms. The tvnorms value is used by the core to implement
     * VIDIOC_ENUMSTD so it has to be correct. If tvnorms == 0, then
     * ENUMSTD will return -ENODATA.
     */
    skel->vdev.tvnorms = i ? 0 : SKEL_TVNORMS;

    /* Update the internal format */
    skeleton_fill_pix_format(skel, &skel->format);
    return 0;
}

static int skeleton_g_input(struct file *file, void *priv, unsigned int *i)
{
    struct skeleton *skel = video_drvdata(file);

    *i = skel->input;
    return 0;
}


// ========================
static const struct vb2_ops skel_qops = {
    .queue_setup		= queue_setup,
    .buf_prepare		= buffer_prepare,
    .buf_queue		= buffer_queue,
    .start_streaming	= start_streaming,
    .stop_streaming		= stop_streaming,
    .wait_prepare		= vb2_ops_wait_prepare,
    .wait_finish		= vb2_ops_wait_finish,
};

static int mmap_wrap(struct file *f, struct vm_area_struct *vas) {
    return vb2_fop_mmap(f, vas);
}

static const struct v4l2_file_operations skel_fops = {
    .owner = THIS_MODULE,
    .open = v4l2_fh_open,
    .release = vb2_fop_release,
    .unlocked_ioctl = video_ioctl2,
    .read = vb2_fop_read,
    .mmap = mmap_wrap,
    .poll = vb2_fop_poll,
};

static int reqbufs_wrap(struct file *file, void *fh, struct v4l2_requestbuffers *b) {
    // 
    b->count = 8;
    printk(KERN_INFO "REQBUFS %d", b->count);
    int ret =  vb2_ioctl_reqbufs(file, fh, b);

    int i;
    for (i = 0; i < skel->queue.num_buffers; i++) {
        struct vb2_buffer* vb = skel->queue.bufs[i];
        dma_addr_t mem = vb2_dma_contig_plane_dma_addr(vb, 0);
        printk(KERN_INFO "BUF[%d]: %dB -> 0x%p\n", i, vb->planes[0].length, (void*)mem);
    }

    return ret;
}

static int create_bufswrap(struct file *file, void *priv, struct v4l2_create_buffers *p) {
    printk(KERN_INFO "CREATEBUFS");
    return vb2_ioctl_create_bufs(file, priv, p);
}

static const struct v4l2_ioctl_ops skel_ioctl_ops = {
    .vidioc_querycap = skeleton_querycap,
    .vidioc_try_fmt_vid_cap = skeleton_try_fmt_vid_cap,
    .vidioc_s_fmt_vid_cap = skeleton_s_fmt_vid_cap,
    .vidioc_g_fmt_vid_cap = skeleton_g_fmt_vid_cap,
    .vidioc_enum_fmt_vid_cap = skeleton_enum_fmt_vid_cap,

    .vidioc_g_std = skeleton_g_std,
    .vidioc_s_std = skeleton_s_std,
    .vidioc_querystd = skeleton_querystd,

    .vidioc_s_dv_timings = skeleton_s_dv_timings,
    .vidioc_g_dv_timings = skeleton_g_dv_timings,
    .vidioc_enum_dv_timings = skeleton_enum_dv_timings,
    .vidioc_query_dv_timings = skeleton_query_dv_timings,
    .vidioc_dv_timings_cap = skeleton_dv_timings_cap,

    .vidioc_enum_input = skeleton_enum_input,
    .vidioc_g_input = skeleton_g_input,
    .vidioc_s_input = skeleton_s_input,

    .vidioc_reqbufs = reqbufs_wrap,
    .vidioc_create_bufs = create_bufswrap,
    .vidioc_querybuf = vb2_ioctl_querybuf,
    .vidioc_qbuf = vb2_ioctl_qbuf,
    .vidioc_dqbuf = vb2_ioctl_dqbuf,
    .vidioc_expbuf = vb2_ioctl_expbuf,
    .vidioc_streamon = vb2_ioctl_streamon,
    .vidioc_streamoff = vb2_ioctl_streamoff,

    .vidioc_log_status = v4l2_ctrl_log_status,
    .vidioc_subscribe_event = v4l2_ctrl_subscribe_event,
    .vidioc_unsubscribe_event = v4l2_event_unsubscribe,
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
    frame_buffer_va = dma_alloc_coherent(dev, PIXELS, &frame_buffer_pa, GFP_KERNEL);
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

    ret = init_cam_i2c();
    if (ret < 0) {
        dev_err(dev, "Init camera i2c failed: %d.\n", ret);
        goto error_i2c;
    }

    /* Init the camera control GPIO */
    ret = init_cam_gpio();
    if (ret < 0) {
        dev_err(dev, "Init camera gpio failed: %d.\n", ret);
        goto error_gpio;
    }

    /* enable the camera via gpio */
    camera_enable();

    /* Detect image sensor model */
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

    /* Init camera regs via I2C */
    ret = init_camera_regs(startup_regs);
    if (ret < 0) {
        dev_err(dev, "init regs using i2c failed\n");
        goto error_i2c_rw;
    }

    /* Add sysfs control interface */
    ret = sysfs_create_groups(&dev->kobj, ar013x_groups);
    if (ret) {
        dev_err(dev, "Registration failed.\n");
        goto error_sysfs;
    }

    /* add misc device for file ops */
    miscdev.fops = &prucam_fops;
    miscdev.minor = MISC_DYNAMIC_MINOR;
    miscdev.mode = S_IRUGO;
    miscdev.name = "prucam";
    ret = misc_register(&miscdev);
    if (ret)
        goto error_misc;

    mutex_init(&cam_mutex);
    printk("prucam probe complete");
    // ========================
    //
    /* The initial timings are chosen to be 720p60. */
    static const struct v4l2_dv_timings timings_def =
        V4L2_DV_BT_CEA_1280X720P60;
    struct video_device *vdev;
    struct v4l2_ctrl_handler *hdl;
    struct vb2_queue *q;

    /* Allocate a new instance */
    skel = devm_kzalloc(&pdev->dev, sizeof(struct skeleton), GFP_KERNEL);
    if (!skel) {
        ret = -ENOMEM;
        goto disable_pci;
    }

    skel->pdev = pdev;

    /* Fill in the initial format-related settings */
    skel->timings = timings_def;
    skel->std = V4L2_STD_625_50;
    skeleton_fill_pix_format(skel, &skel->format);

    /* Initialize the top-level structure */
    ret = v4l2_device_register(&pdev->dev, &skel->v4l2_dev);
    if (ret)
        goto disable_pci;

    mutex_init(&skel->lock);

    /* Add the controls */
    hdl = &skel->ctrl_handler;
    v4l2_ctrl_handler_init(hdl, 4);
    v4l2_ctrl_new_std(hdl, &skel_ctrl_ops,
            V4L2_CID_BRIGHTNESS, 0, 255, 1, 127);
    v4l2_ctrl_new_std(hdl, &skel_ctrl_ops,
            V4L2_CID_CONTRAST, 0, 255, 1, 16);
    v4l2_ctrl_new_std(hdl, &skel_ctrl_ops,
            V4L2_CID_SATURATION, 0, 255, 1, 127);
    v4l2_ctrl_new_std(hdl, &skel_ctrl_ops,
            V4L2_CID_HUE, -128, 127, 1, 0);
    if (hdl->error) {
        ret = hdl->error;
        goto free_hdl;
    }
    skel->v4l2_dev.ctrl_handler = hdl;

    /* Initialize the vb2 queue */
    q = &skel->queue;
    q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    q->io_modes = VB2_MMAP;
    //q->io_modes = VB2_DMABUF;
    //q->io_modes = VB2_MMAP | VB2_DMABUF | VB2_READ;
    q->dev = &pdev->dev;
    q->drv_priv = skel;
    q->buf_struct_size = sizeof(struct skel_buffer);
    q->ops = &skel_qops;
    q->mem_ops = &vb2_dma_contig_memops;
    q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
    /*
     * Assume that this DMA engine needs to have at least two buffers
     * available before it can be started. The start_streaming() op
     * won't be called until at least this many buffers are queued up.
     */
    q->min_buffers_needed = 2;
    /*
     * The serialization lock for the streaming ioctls. This is the same
     * as the main serialization lock, but if some of the non-streaming
     * ioctls could take a long time to execute, then you might want to
     * have a different lock here to prevent VIDIOC_DQBUF from being
     * blocked while waiting for another action to finish. This is
     * generally not needed for PCI devices, but USB devices usually do
     * want a separate lock here.
     */
    q->lock = &skel->lock;
    /*
     * Since this driver can only do 32-bit DMA we must make sure that
     * the vb2 core will allocate the buffers in 32-bit DMA memory.
     */
    q->gfp_flags = GFP_DMA32;


    // Oliver: set memory type to DMA buf
    q->memory = VB2_MEMORY_DMABUF;

    ret = vb2_queue_init(q);
    if (ret)
        goto free_hdl;

    INIT_LIST_HEAD(&skel->buf_list);
    spin_lock_init(&skel->qlock);

    /* Initialize the video_device structure */
    vdev = &skel->vdev;
    strlcpy(vdev->name, KBUILD_MODNAME, sizeof(vdev->name));
    /*
     * There is nothing to clean up, so release is set to an empty release
     * function. The release callback must be non-NULL.
     */
    vdev->release = video_device_release_empty;
    vdev->fops = &skel_fops,
        vdev->ioctl_ops = &skel_ioctl_ops,
        vdev->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE |
            V4L2_CAP_STREAMING;
    /*
     * The main serialization lock. All ioctls are serialized by this
     * lock. Exception: if q->lock is set, then the streaming ioctls
     * are serialized by that separate lock.
     */
    vdev->lock = &skel->lock;
    vdev->queue = q;
    vdev->v4l2_dev = &skel->v4l2_dev;
    /* Supported SDTV standards, if any */
    vdev->tvnorms = SKEL_TVNORMS;
    video_set_drvdata(vdev, skel);

    ret = video_register_device(vdev, VFL_TYPE_VIDEO, -1);
    if (ret)
        goto free_hdl;

    dev_info(&pdev->dev, "V4L2Driver loaded\n");
    return 0;


disable_pci:
free_hdl:
    v4l2_ctrl_handler_free(&skel->ctrl_handler);
    v4l2_device_unregister(&skel->v4l2_dev);


    // ========================

    return 0;

error_misc:
    sysfs_remove_groups(&dev->kobj, ar013x_groups);
error_sysfs:
error_i2c_rw:
    free_cam_gpio();
error_gpio:
    end_cam_i2c();
error_i2c:
    dma_free_coherent(dev, PIXELS, frame_buffer_va, frame_buffer_pa);
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

    misc_deregister(&miscdev);

    /* Remove the sysfs attr */
    sysfs_remove_groups(&dev->kobj, ar013x_groups);

    /* Put camera GPIO in good state and free the lines */
    free_cam_gpio();

    end_cam_i2c();

    dma_free_coherent(dev, PIXELS, frame_buffer_va, frame_buffer_pa);

    /* Free the shared mem region and pruss */
    pruss_release_mem_region(pruss, &shared_mem);
    pruss_put(pruss);

    /* Stop PRUs */
    rproc_shutdown(pru0);
    rproc_shutdown(pru1);


    free_irqs();

    pru_rproc_put(pru1);
    pru_rproc_put(pru0);

    video_unregister_device(&skel->vdev);
    v4l2_ctrl_handler_free(&skel->ctrl_handler);
    v4l2_device_unregister(&skel->v4l2_dev);

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

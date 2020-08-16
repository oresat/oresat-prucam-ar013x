#include <linux/module.h>         // Core header for loading LKMs into the kernel

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Oliver Rew");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("write this");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

#define DEVICE_NAME     "prucam"    ///< The device will appear at /dev/prucam using this value
#define CLASS_NAME      "pru"        ///< The device class -- this is a character device driver
#define ROWS            960
#define COLS            1280
#define SIZE            (ROWS * COLS)
#define PIXELS          (ROWS * COLS)
#define PRUBASE         0x4a300000
#define PRUSHAREDRAM    (PRUBASE + 0x10000)
#define PRUINTC_OFFSET  0x20000
#define SRSR0_OFFSET    0x200
#define NUM_IRQS        8



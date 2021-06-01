#ifndef CAM_GPIO_H
#define CAM_GPIO_H

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/module.h>

// Camera Control Pin Numbers
#define GPIO_FLASH    16
#define GPIO_BUS_OE   97
#define GPIO_CAM_OE   96
#define GPIO_CLK_EN   106
#define GPIO_INPUT_EN 85
#define GPIO_RESET    84
#define GPIO_SADDR    83
#define GPIO_STANDBY  82
#define GPIO_TRIGGER  98
#define GPIO_VREG_EN  100

typedef struct {
    uint8_t num;
    bool init;
    bool enable;
    char *label;
} gpio;

/**
 * @brief Initalize all the camera gpio pins..
 * @return 0 on success or negative errno on error.
 */
int init_cam_gpio(void);

/**
 * @brief Enables the gpio pins for camera.
 * @return 0 on success or negative errno on error.
 */
void camera_enable(void);

/**
 * @brief Puts the camera gpio pins in a good, disabled state and frees the
 * lines.
 * @return 0 on success or negative errno on error.
 */
int free_cam_gpio(void);

#endif

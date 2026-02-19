#ifndef CAM_GPIO_H
#define CAM_GPIO_H

#include <linux/gpio/consumer.h>

/**
 * @brief Initalize all the camera gpio pins..
 * @return 0 on success or negative errno on error.
 */
int init_cam_gpio(struct device *dev);

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
void free_cam_gpio(struct device *dev);

#endif

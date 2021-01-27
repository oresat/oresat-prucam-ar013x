/**
 * @file    ar013x_sysfs.h
 * @brief   AR013x CMOS Digital Image Sensor sysfs.
 *
 * @addtogroup AR013x
 */

#ifndef AR013X_SYSFS_H
#define AR013X_SYSFS_H

#include <linux/sysfs.h>

#define CONTEXT_A       0
#define CONTEXT_B       1

/** 
 * @brief Gets the size of the images.
 * @return length of attribute on succes or negative errno value on failure.
 */
ssize_t ar013x_img_size_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets the size of the image
 * @return Attribute's count on succes or negative errno value on failure.
 */
ssize_t ar013x_img_size_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/**
 * @brief Gets all the color gains (green1, red, blue, green2, & gobal gains)
 * @return length of attribute on succes or negative errno value on failure.
 */
ssize_t ar013x_color_gain_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets all the color gains (green1, red, blue, green2, & gobal gains)
 * @return Attribute's count on succes or negative errno value on failure.
 */
ssize_t ar013x_color_gain_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/** 
 * @brief Gets the bit fields in the digital_test register (contain the context selector and analog gain)
 * @return length of attribute on succes or negative errno value on failure.
 */
ssize_t ar013x_digital_test_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets the bit fields in the digital_test register (contain the context selector and analog gain)
 * @return Attribute's count on succes or negative errno value on failure.
 */
ssize_t ar013x_digital_test_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/**
 * @brief Gets y odd inc values
 * @return length of attribute on succes or negative errno value on failure.
 */
ssize_t ar013x_y_odd_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets y odd inc values
 * @return Attribute's count on succes or negative errno value on failure.
 */
ssize_t ar013x_y_odd_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/**
 * @brief Gets the digital binning values
 * @return length of attribute on succes or negative errno value on failure.
 */
ssize_t ar013x_digital_binning_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets the digital binning values
 * @return Attribute's count on succes or negative errno value on failure.
 */
ssize_t ar013x_digital_binning_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/**
 * @brief Gets general registers with no bit masking (all 16 bits of the register are used)
 * @return length of attribute on succes or negative errno value on failure.
 */
ssize_t ar013x_general_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets general registers with no bit masking (all 16 bits of the register are used)
 * @return Attribute's count on succes or negative errno value on failure.
 */
ssize_t ar013x_general_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

DEVICE_ATTR(context, S_IRUGO | S_IWUSR, ar013x_digital_test_show, ar013x_digital_test_store);
DEVICE_ATTR(x_size, S_IRUGO | S_IWUSR, ar013x_img_size_show, ar013x_img_size_store);
DEVICE_ATTR(y_size, S_IRUGO | S_IWUSR, ar013x_img_size_show, ar013x_img_size_store);
DEVICE_ATTR(coarse_time, S_IRUGO | S_IWUSR, ar013x_general_show, ar013x_general_store);
DEVICE_ATTR(fine_time, S_IRUGO | S_IWUSR, ar013x_general_show, ar013x_general_store);
DEVICE_ATTR(y_odd_inc, S_IRUGO | S_IWUSR, ar013x_y_odd_show, ar013x_y_odd_store);
DEVICE_ATTR(green1_gain, S_IRUGO | S_IWUSR, ar013x_color_gain_show, ar013x_color_gain_store);
DEVICE_ATTR(blue_gain, S_IRUGO | S_IWUSR, ar013x_color_gain_show, ar013x_color_gain_store);
DEVICE_ATTR(red_gain, S_IRUGO | S_IWUSR, ar013x_color_gain_show, ar013x_color_gain_store);
DEVICE_ATTR(green2_gain, S_IRUGO | S_IWUSR, ar013x_color_gain_show, ar013x_color_gain_store);
DEVICE_ATTR(global_gain, S_IRUGO | S_IWUSR, ar013x_color_gain_show, ar013x_color_gain_store);
DEVICE_ATTR(analog_gain, S_IRUGO | S_IWUSR, ar013x_digital_test_show, ar013x_digital_test_store);
DEVICE_ATTR(frame_len_lines, S_IRUGO | S_IWUSR, ar013x_general_show, ar013x_general_store);
DEVICE_ATTR(digital_binning, S_IRUGO | S_IWUSR, ar013x_digital_binning_show, ar013x_digital_binning_store);

struct attribute *ar013x_attrs[] = {
    &dev_attr_context.attr,
    &dev_attr_x_size.attr,
    &dev_attr_y_size.attr,
    &dev_attr_coarse_time.attr,
    &dev_attr_fine_time.attr,
    &dev_attr_y_odd_inc.attr,
    &dev_attr_green1_gain.attr,
    &dev_attr_blue_gain.attr,
    &dev_attr_red_gain.attr,
    &dev_attr_green2_gain.attr,
    &dev_attr_global_gain.attr,
    &dev_attr_analog_gain.attr,
    &dev_attr_frame_len_lines.attr,
    &dev_attr_digital_binning.attr,
    NULL
};

struct attribute_group ar013x_group = {
    .name = "context_settings",
    .attrs = ar013x_attrs,
};

const struct attribute_group *ar013x_groups[] = {
    &ar013x_group,
    NULL
};

#endif /* AR013X_SYSFS_H */

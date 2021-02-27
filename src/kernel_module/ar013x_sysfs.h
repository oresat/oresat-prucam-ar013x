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

#define ENABLE    0
#define DISABLE   1

// ---------------------------------------------------------------------------
// Context registers

/** 
 * @brief Gets the size of the images.
 * @return length of attribute on success or negative errno value on failure.
 */
ssize_t ar013x_img_size_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets the size of the image
 * @return Attribute's count on success or negative errno value on failure.
 */
ssize_t ar013x_img_size_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/**
 * @brief Gets all the color gains (green1, red, blue, green2, & gobal gains)
 * @return length of attribute on success or negative errno value on failure.
 */
ssize_t ar013x_color_gain_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets all the color gains (green1, red, blue, green2, & gobal gains)
 * @return Attribute's count on success or negative errno value on failure.
 */
ssize_t ar013x_color_gain_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/** 
 * @brief Gets the bit fields in the digital_test register (contain the context selector and analog gain)
 * @return length of attribute on success or negative errno value on failure.
 */
ssize_t ar013x_digital_test_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets the bit fields in the digital_test register (contain the context selector and analog gain)
 * @return Attribute's count on success or negative errno value on failure.
 */
ssize_t ar013x_digital_test_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/**
 * @brief Gets y odd inc values
 * @return length of attribute on success or negative errno value on failure.
 */
ssize_t ar013x_y_odd_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets y odd inc values
 * @return Attribute's count on success or negative errno value on failure.
 */
ssize_t ar013x_y_odd_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/**
 * @brief Gets the digital binning values
 * @return length of attribute on success or negative errno value on failure.
 */
ssize_t ar013x_digital_binning_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets the digital binning values
 * @return Attribute's count on success or negative errno value on failure.
 */
ssize_t ar013x_digital_binning_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/**
 * @brief Gets general registers with no bit masking (all 16 bits of the register are used)
 * @return length of attribute on success or negative errno value on failure.
 */
ssize_t ar013x_general_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets general registers with no bit masking (all 16 bits of the register are used)
 * @return Attribute's count on success or negative errno value on failure.
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

struct attribute *ar013x_context_attrs[] = {
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

struct attribute_group ar013x_context_group = {
    .name = "context_settings",
    .attrs = ar013x_context_attrs,
};

// ---------------------------------------------------------------------------
// Auto exposure

/**
 * @brief Gets the value from the auto exposure register.
 * @return length of attribute on success or negative errno value on failure.
 */
ssize_t ar013x_auto_exposure_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets the auto exposure register.
 * @return Attribute's count on success or negative errno value on failure.
 */
ssize_t ar013x_auto_exposure_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

/**
 * @brief Gets the value from any of the auto exposure registers.
 * @return length of attribute on success or negative errno value on failure.
 */
ssize_t ar013x_ae_general_show(struct device *dev, struct device_attribute *attr, char *buf);

/**
 * @brief Sets any of the auto exposure registers.
 * @return Attribute's count on success or negative errno value on failure.
 */
ssize_t ar013x_ae_general_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

DEVICE_ATTR(ae_enable, S_IRUGO | S_IWUSR, ar013x_auto_exposure_show, ar013x_auto_exposure_store);
DEVICE_ATTR(ae_ag_en, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_dg_en, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_min_ana_gain, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_roi_x_start_offset, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_roi_y_start_offset, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_roi_x_size, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_roi_y_size, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_luma_target, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_min_ev_step, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_max_ev_step, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_damp_offset, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_damp_gain, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_damp_max, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_max_exposure, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_min_exposure, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_ag_exposure_hi, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_ag_exposure_lo, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);
DEVICE_ATTR(ae_dark_cur_thresh, S_IRUGO | S_IWUSR, ar013x_ae_general_show, ar013x_ae_general_store);

struct attribute *ar013x_auto_exposure_attrs[] = {
    &dev_attr_ae_enable.attr,
    &dev_attr_ae_ag_en.attr,
    &dev_attr_ae_dg_en.attr,
    &dev_attr_ae_min_ana_gain.attr,
    &dev_attr_ae_roi_x_start_offset.attr,
    &dev_attr_ae_roi_y_start_offset.attr,
    &dev_attr_ae_roi_x_size.attr,
    &dev_attr_ae_roi_y_size.attr,
    &dev_attr_ae_luma_target.attr,
    &dev_attr_ae_min_ev_step.attr,
    &dev_attr_ae_max_ev_step.attr,
    &dev_attr_ae_damp_offset.attr,
    &dev_attr_ae_damp_gain.attr,
    &dev_attr_ae_damp_max.attr,
    &dev_attr_ae_max_exposure.attr,
    &dev_attr_ae_min_exposure.attr,
    &dev_attr_ae_ag_exposure_hi.attr,
    &dev_attr_ae_ag_exposure_lo.attr,
    &dev_attr_ae_dark_cur_thresh.attr,
    NULL
};

struct attribute_group ar013x_auto_exposure_group = {
    .name = "auto_exposure_settings",
    .attrs = ar013x_auto_exposure_attrs,
};

// ---------------------------------------------------------------------------
// groups

const struct attribute_group *ar013x_groups[] = {
    &ar013x_context_group,
    &ar013x_auto_exposure_group,
    NULL
};

#endif /* AR013X_SYSFS_H */

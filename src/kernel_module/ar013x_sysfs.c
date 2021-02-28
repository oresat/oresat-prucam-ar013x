/**
 * @file    ar013x_sysfs.c
 * @brief   AR013x CMOS Digital Image Sensor sysfs.
 *
 * @addtogroup AR013x
 */

#include "ar013x_regs.h"
#include "cam_i2c.h"
#include <linux/sysfs.h>

#define CONTEXT_A       0
#define CONTEXT_B       1

/** @brief Local flag for the current context */
static int context = CONTEXT_A;

/** 
 * @brief Gets the correct context register based
 * @param reg The register #define for Context A
 * @return The register #define for the current context
 * */
#define CONTEXT_REG(reg) (context == CONTEXT_A ? reg : reg ## _CB)

ssize_t ar013x_img_size_show(struct device *dev, struct device_attribute *attr, char *buf) {
    uint16_t start_value = 0, end_value = 0, start_reg, end_reg;
    int len = 0, r;

    if (strcmp(attr->attr.name, "x_size") == 0) {
        start_reg = CONTEXT_REG(AR013X_AD_X_ADDR_START);
        end_reg = CONTEXT_REG(AR013X_AD_X_ADDR_END);
    } else if (strcmp(attr->attr.name, "y_size") == 0) {
        start_reg = CONTEXT_REG(AR013X_AD_Y_ADDR_START);
        end_reg = CONTEXT_REG(AR013X_AD_Y_ADDR_END);
    } else {
        dev_err(dev, "prucam: unknown show name %s", attr->attr.name);
        return len;
    }

    r = read_cam_reg(start_reg, &start_value);
    if (r != 0)
        return r;

    r = read_cam_reg(end_reg, &end_value);
    if (r != 0)
        return r;

    len = sprintf(buf, "%d\n", end_value - start_value + 1);
    if (len <= 0)
        dev_err(dev, "prucam: invalid sprintf len %d", len);

    return len;
}

ssize_t ar013x_img_size_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    uint16_t start_value, value, start_reg, end_reg, end_reg_max;
    int temp, r;

    if ((r = kstrtoint(buf, 10, &temp)) < 0) {
        dev_err(dev, "prucam: %s store value was not a interger", attr->attr.name);
        return r;
    }

    value = (uint16_t)temp;

    if (strcmp(attr->attr.name, "x_size") == 0) {
        dev_err(dev, "prucam: %s store value was not a interger", attr->attr.name);
        start_reg = CONTEXT_REG(AR013X_AD_X_ADDR_START);
        end_reg = CONTEXT_REG(AR013X_AD_X_ADDR_END);
        end_reg_max = 0x07FF;
    } else if (strcmp(attr->attr.name, "y_size") == 0) {
        start_reg = CONTEXT_REG(AR013X_AD_Y_ADDR_START);
        end_reg = CONTEXT_REG(AR013X_AD_Y_ADDR_END);
        end_reg_max = 0x03FF;
    } else {
        dev_err(dev, "prucam: unknown store name %s", attr->attr.name);
        return -EINVAL;
    }

    r = read_cam_reg(start_reg, &start_value);
    if (r != 0)
        return r;

    value += start_value;
    value -= 1;

    if (value >= end_reg_max) {
        dev_err(dev, "prucam: %s value is too big", attr->attr.name);
        return -EINVAL;
    }
    
    r = write_cam_reg(end_reg, value);
    if (r != 0)
        return r;

    return count;
}

ssize_t ar013x_color_gain_show(struct device *dev, struct device_attribute *attr, char *buf) {
    uint16_t value = 0, reg;
    int len = 0, r;

    if (strcmp(attr->attr.name, "green1_gain") == 0) {
        reg = CONTEXT_REG(AR013X_AD_GREEN1_GAIN);
    } else if (strcmp(attr->attr.name, "blue_gain") == 0) {
        reg = CONTEXT_REG(AR013X_AD_BLUE_GAIN);
    } else if (strcmp(attr->attr.name, "red_gain") == 0) {
        reg = CONTEXT_REG(AR013X_AD_RED_GAIN);
    } else if (strcmp(attr->attr.name, "green2_gain") == 0) {
        reg = CONTEXT_REG(AR013X_AD_GREEN2_GAIN);
    } else if (strcmp(attr->attr.name, "global_gain") == 0) {
        reg = CONTEXT_REG(AR013X_AD_GLOBAL_GAIN);
    } else {
        dev_err(dev, "prucam: unknown show name %s", attr->attr.name);
        return len;
    }

    r = read_cam_reg(reg, &value);
    if (r != 0)
        return r;

    value &= 0x00FF;

    len = sprintf(buf, "%d\n", (int)value);
    if (len <= 0)
        dev_err(dev, "prucam: Invalid sprintf len: %d\n", len);

    return len;
}

ssize_t ar013x_color_gain_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    uint16_t value, reg;
    int temp, r;

    if ((r = kstrtoint(buf, 10, &temp)) < 0) {
        dev_err(dev, "prucam: %s store value was not a interger", attr->attr.name);
        return r;
    }

    value = (uint16_t)temp;

    if (strcmp(attr->attr.name, "green1_gain") == 0) {
        reg = CONTEXT_REG(AR013X_AD_GREEN1_GAIN);
    } else if (strcmp(attr->attr.name, "blue_gain") == 0) {
        reg = CONTEXT_REG(AR013X_AD_BLUE_GAIN);
    } else if (strcmp(attr->attr.name, "red_gain") == 0) {
        reg = CONTEXT_REG(AR013X_AD_RED_GAIN);
    } else if (strcmp(attr->attr.name, "green2_gain") == 0) {
        reg = CONTEXT_REG(AR013X_AD_GREEN2_GAIN);
    } else if (strcmp(attr->attr.name, "global_gain") == 0) {
        reg = CONTEXT_REG(AR013X_AD_GLOBAL_GAIN);
    } else {
        dev_err(dev, "prucam: unknown store name %s", attr->attr.name);
        return -EINVAL;
    }

    if (value >= 0x00FF) {
        dev_err(dev, "prucam: %s value is too big", attr->attr.name);
        return -EINVAL;
    }
    
    r = write_cam_reg(reg, value);
    if (r != 0)
        return r;

    return count;
}

ssize_t ar013x_y_odd_show(struct device *dev, struct device_attribute *attr, char *buf) {
    uint16_t value = 0, reg;
    int len = 0, r;

    reg = CONTEXT_REG(AR013X_AD_Y_ODD_INC);

    r = read_cam_reg(reg, &value);
    if (r != 0)
        return r;

    value &= 0x007F;

    len = sprintf(buf, "%d\n", value);
    if (len <= 0)
        dev_err(dev, "prucam: invalid sprintf len %d", len);

    return len;
}

ssize_t ar013x_y_odd_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    uint16_t value, reg;
    int temp, r;

    if ((r = kstrtoint(buf, 10, &temp)) < 0) {
        dev_err(dev, "prucam: %s store value was not a interger", attr->attr.name);
        return r;
    }

    value = (uint16_t)temp;

    if (value >= 0x007F) {
        dev_err(dev, "prucam: %s must be <= 0x007F", attr->attr.name);
        return -EINVAL;
    }

    reg = CONTEXT_REG(AR013X_AD_Y_ODD_INC);

    r = write_cam_reg(reg, value);
    if (r != 0)
        return r;

    return count;
}

ssize_t ar013x_digital_test_show(struct device *dev, struct device_attribute *attr, char *buf) {
    uint16_t value = 0;
    int len = 0, r;

    r = read_cam_reg(AR013X_AD_DIGITAL_TEST, &value);
    if (r != 0)
        return r;

    if (strcmp(attr->attr.name, "context") == 0) {
        // Context switch is bit 13
        value >>= 13;
        value &= 0x0001;
    } else if (strcmp(attr->attr.name, "analog_gain") == 0) {
        // Context A is bits [5:4] & Context B is bits [9:8]
        value >>= context == CONTEXT_A ? 4 : 8;
        value &= 0x0003;
    } 

    len = sprintf(buf, "%d\n", value);
    if (len <= 0)
        dev_err(dev, "prucam: invalid sprintf len %d", len);

    return len;
}

ssize_t ar013x_digital_test_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    uint16_t input_value, reg_value;
    int temp, r;

    if ((r = kstrtoint(buf, 10, &temp)) < 0) {
        dev_err(dev, "prucam: %s store value was not a interger", attr->attr.name);
        return r;
    }

    input_value = (uint16_t)temp;

    r = read_cam_reg(AR013X_AD_DIGITAL_TEST, &reg_value);
    if (r != 0)
        return r;
    
    if (strcmp(attr->attr.name, "context") == 0) {
        if (input_value != 0 && input_value != 1) { // 1 bit
            dev_err(dev, "prucam: %s attr must be 0 or 1 not %d", attr->attr.name, input_value);
            return -EINVAL;
        }

        // Context switch is bit 13
        input_value <<= 13;
        input_value &= 0x2000;
        reg_value &= 0xDFFF;
    } else if (strcmp(attr->attr.name, "analog_gain") == 0) {
        if (input_value > 0x3) { // 2 bits
            dev_err(dev, "prucam: %s must be <= 0x3", attr->attr.name);
            return -EINVAL;
        }

        if (context == CONTEXT_A) { // Context A is bits [5:4]
            input_value <<= 4;
            input_value &= 0x0030;
            reg_value &= 0xFFCF;
        } else { // Context B is bits [9:8]
            input_value <<= 8;
            input_value &= 0x0300;
            reg_value &= 0xFCFF;
        }
    } 
    
    reg_value |= input_value;

    r = write_cam_reg(AR013X_AD_DIGITAL_TEST, reg_value);
    if (r != 0)
        return r;

    return count;
}

ssize_t ar013x_digital_binning_show(struct device *dev, struct device_attribute *attr, char *buf) {
    uint16_t value = 0;
    int len = 0, r;

    r = read_cam_reg(AR013X_AD_DIGITAL_BINNING, &value);
    if (r != 0)
        return r;

    // 2 bits, Context A is [1:0] & Context B is [5:4]
    if (context == CONTEXT_B) 
        value >>= 4;

    value &= 0x0003;

    len = sprintf(buf, "%d\n", value);
    if (len <= 0)
        dev_err(dev, "prucam: invalid sprintf len %d", len);

    return len;
}

ssize_t ar013x_digital_binning_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    uint16_t input_value, reg_value;
    int temp, r;

    if ((r = kstrtoint(buf, 10, &temp)) < 0) {
        dev_err(dev, "prucam: %s store value was not a interger", attr->attr.name);
        return r;
    }

    input_value = (uint16_t)temp;

    if (input_value > 0x3) { // 2 bits
        dev_err(dev, "prucam: %s must be <= 0x3", attr->attr.name);
        return -EINVAL;
    }

    r = read_cam_reg(AR013X_AD_DIGITAL_BINNING, &reg_value);
    if (r != 0)
        return r;

    // 2 bits, Context A is [1:0] & Context B is [5:4]
    if (context == CONTEXT_A) {
        input_value &= 0x0003;
        reg_value  &= 0xFFFC;
    } else {
        input_value <<= 4;
        input_value &= 0x0030;
        reg_value &= 0xFFCF;
    }

    reg_value |= input_value;
    
    r = write_cam_reg(AR013X_AD_DIGITAL_BINNING, reg_value);
    if (r != 0)
        return r;

    return count;
}

ssize_t ar013x_general_show(struct device *dev, struct device_attribute *attr, char *buf) {
    uint16_t value = 0, reg = 0;
    int len = 0, r;

    if (strcmp(attr->attr.name, "coarse_time") == 0)
        reg = CONTEXT_REG(AR013X_AD_COARSE_INTEGRATION_TIME);
    else if (strcmp(attr->attr.name, "fine_time") == 0)
        reg = CONTEXT_REG(AR013X_AD_FINE_INTERGRATION_TIME);
    else if (strcmp(attr->attr.name, "frame_len_lines") == 0)
        reg = CONTEXT_REG(AR013X_AD_FRAME_LEN_LINES);
    else
        dev_err(dev, "prucam: unknown show name %s", attr->attr.name);

    r = read_cam_reg(reg, &value);
    if (r != 0)
        return r;

    len = sprintf(buf, "%d\n", value);
    if (len <= 0)
        dev_err(dev, "prucam: invalid sprintf len %d", len);

    return len;
}

ssize_t ar013x_general_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    uint16_t value, reg = 0;
    int temp, r;

    if ((r = kstrtoint(buf, 10, &temp)) < 0) {
        dev_err(dev, "prucam: %s store value was not a interger", attr->attr.name);
        return r;
    }

    value = (uint16_t)temp;

    if (strcmp(attr->attr.name, "coarse_time") == 0)
        reg = CONTEXT_REG(AR013X_AD_COARSE_INTEGRATION_TIME);
    else if (strcmp(attr->attr.name, "fine_time") == 0)
        reg = CONTEXT_REG(AR013X_AD_FINE_INTERGRATION_TIME);
    else if (strcmp(attr->attr.name, "frame_len_lines") == 0)
        reg = CONTEXT_REG(AR013X_AD_FRAME_LEN_LINES);
    else
        dev_err(dev, "prucam: unknown store name %s", attr->attr.name);

    r = write_cam_reg(reg, value);
    if (r != 0)
        return r;

    return count;
}

// ---------------------------------------------------------------------------
// Auto exposure

ssize_t ar013x_auto_exposure_show(struct device *dev, struct device_attribute *attr, char *buf) {
    uint16_t value = 0;
    int len = 0, r;

    r = read_cam_reg(AR013X_AD_AE_CTRL_REG, &value);
    if (r != 0)
        return r;

    if (strcmp(attr->attr.name, "min_analog_gain") == 0) { // bits [6:5]
        value >>= 5; 
        value &= 0x3;
    } else if (strcmp(attr->attr.name, "auto_dg_en") == 0) { // bit [4]
        value >>= 4;
        value &= 0x1;
    } else if (strcmp(attr->attr.name, "auto_gd_en") == 0) { // bit [1]
        value >>= 1;
        value &= 0x1;
    } else if (strcmp(attr->attr.name, "ae_enable") == 0) {// bit [0]
        value &= 0x1;
    }

    len = sprintf(buf, "%d\n", value);
    if (len <= 0)
        dev_err(dev, "prucam: invalid sprintf len %d", len);

    return len;
}

ssize_t ar013x_auto_exposure_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    uint16_t input_value, reg_value;
    int temp, r, size = 0;

    if ((r = kstrtoint(buf, 10, &temp)) < 0) {
        dev_err(dev, "prucam: %s store value was not a interger", attr->attr.name);
        return r;
    }

    input_value = (uint16_t)temp;

    // make sure valid size
    if (strcmp(attr->attr.name, "min_analog_gain") == 0)
        size = 0x3; // bits [6:5]
    else
        size = 0x1; // everthing else is 1 bit

    if (input_value > size) { 
        dev_err(dev, "prucam: %s must be <= %d", attr->attr.name, size);
        return -EINVAL;
    }

    r = read_cam_reg(AR013X_AD_AE_CTRL_REG, &reg_value);
    if (r != 0)
        return r;

    if (strcmp(attr->attr.name, "min_analog_gain") == 0) { // bits [6:5]
        input_value <<= 5;
        input_value &= 0x0060;
        reg_value &= 0xFF8F;
    } else if (strcmp(attr->attr.name, "auto_dg_en") == 0) { // bit [4]
        input_value <<= 4;
        input_value &= 0x0010;
        reg_value &= 0xFFEF;
    } else if (strcmp(attr->attr.name, "auto_gd_en") == 0) { // bit [1]
        input_value <<= 1;
        input_value &= 0x0002;
        reg_value &= 0xFFFD;
    } else if (strcmp(attr->attr.name, "ae_enable") == 0) {// bit [0]
        input_value &= 0x0001;
        reg_value &= 0xFFFE;

        // disable digital binning
        if((r = write_cam_reg(AR013X_AD_DIGITAL_BINNING, 0)) != 0)
            return r;
    }

    reg_value |= input_value;

    r = write_cam_reg(AR013X_AD_AE_CTRL_REG, reg_value);
    if (r != 0)
        return r;

    return count;
}

ssize_t ar013x_ae_general_show(struct device *dev, struct device_attribute *attr, char *buf) {
    uint16_t value = 0, reg = 0;
    int len = 0, r;

    if (strcmp(attr->attr.name, "ae_roi_x_start_offset") == 0)
        reg = AR013X_AD_AE_ROI_X_START_OFFSET;
    else if (strcmp(attr->attr.name, "ae_roi_y_start_offset") == 0)
        reg = AR013X_AD_AE_ROI_Y_START_OFFSET;
    else if (strcmp(attr->attr.name, "ae_roi_x_size") == 0)
        reg = AR013X_AD_AE_ROI_X_SIZE;
    else if (strcmp(attr->attr.name, "ae_roi_y_size") == 0)
        reg = AR013X_AD_AE_ROI_Y_SIZE;
    else if (strcmp(attr->attr.name, "ae_luma_target") == 0)
        reg = AR013X_AD_AE_LUMA_TARGET_REG;
    else if (strcmp(attr->attr.name, "ae_min_ev_step") == 0)
        reg = AR013X_AD_AE_MIN_EV_STEP_REG;
    else if (strcmp(attr->attr.name, "ae_max_ev_step") == 0)
        reg = AR013X_AD_AE_MAX_EV_STEP_REG;
    else if (strcmp(attr->attr.name, "ae_damp_offset") == 0)
        reg = AR013X_AD_AE_DAMP_OFFSET_REG;
    else if (strcmp(attr->attr.name, "ae_damp_gain") == 0)
        reg = AR013X_AD_AE_DAMP_GAIN_REG;
    else if (strcmp(attr->attr.name, "ae_damp_max") == 0)
        reg = AR013X_AD_AE_DAMP_MAX_REG;
    else if (strcmp(attr->attr.name, "ae_min_exposure") == 0)
        reg = AR013X_AD_AE_MIN_EXPOSURE_REG;
    else if (strcmp(attr->attr.name, "ae_max_exposure") == 0)
        reg = AR013X_AD_AE_MAX_EXPOSURE_REG;
    else if (strcmp(attr->attr.name, "ae_ag_exposure_hi") == 0)
        reg = AR013X_AD_AE_AG_EXPOSURE_HI;
    else if (strcmp(attr->attr.name, "ae_ag_exposure_lo") == 0)
        reg = AR013X_AD_AE_AG_EXPOSURE_LO;
    else if (strcmp(attr->attr.name, "ae_dark_cur_thresh") == 0)
        reg = AR013X_AD_AE_DARK_CUR_THRESH_REG;
    else
        dev_err(dev, "prucam: unknown show name %s", attr->attr.name);

    r = read_cam_reg(reg, &value);
    if (r != 0)
        return r;

    len = sprintf(buf, "%d\n", value);
    if (len <= 0)
        dev_err(dev, "prucam: invalid sprintf len %d", len);

    return len;
}

ssize_t ar013x_ae_general_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
    uint16_t input_value, reg = 0;
    int r, temp;

    if ((r = kstrtoint(buf, 10, &temp)) < 0) {
        dev_err(dev, "prucam: %s store value was not a interger", attr->attr.name);
        return r;
    }

    input_value = (uint16_t)temp;

    if (strcmp(attr->attr.name, "ae_roi_x_start_offset") == 0)
        reg = AR013X_AD_AE_ROI_X_START_OFFSET;
    else if (strcmp(attr->attr.name, "ae_roi_y_start_offset") == 0)
        reg = AR013X_AD_AE_ROI_Y_START_OFFSET;
    else if (strcmp(attr->attr.name, "ae_roi_x_size") == 0)
        reg = AR013X_AD_AE_ROI_X_SIZE;
    else if (strcmp(attr->attr.name, "ae_roi_y_size") == 0)
        reg = AR013X_AD_AE_ROI_Y_SIZE;
    else if (strcmp(attr->attr.name, "ae_luma_target") == 0)
        reg = AR013X_AD_AE_LUMA_TARGET_REG;
    else if (strcmp(attr->attr.name, "ae_min_ev_step") == 0)
        reg = AR013X_AD_AE_MIN_EV_STEP_REG;
    else if (strcmp(attr->attr.name, "ae_max_ev_step") == 0)
        reg = AR013X_AD_AE_MAX_EV_STEP_REG;
    else if (strcmp(attr->attr.name, "ae_damp_offset") == 0)
        reg = AR013X_AD_AE_DAMP_OFFSET_REG;
    else if (strcmp(attr->attr.name, "ae_damp_gain") == 0)
        reg = AR013X_AD_AE_DAMP_GAIN_REG;
    else if (strcmp(attr->attr.name, "ae_damp_max") == 0)
        reg = AR013X_AD_AE_DAMP_MAX_REG;
    else if (strcmp(attr->attr.name, "ae_min_exposure") == 0)
        reg = AR013X_AD_AE_MIN_EXPOSURE_REG;
    else if (strcmp(attr->attr.name, "ae_max_exposure") == 0)
        reg = AR013X_AD_AE_MAX_EXPOSURE_REG;
    else if (strcmp(attr->attr.name, "ae_ag_exposure_hi") == 0)
        reg = AR013X_AD_AE_AG_EXPOSURE_HI;
    else if (strcmp(attr->attr.name, "ae_ag_exposure_lo") == 0)
        reg = AR013X_AD_AE_AG_EXPOSURE_LO;
    else if (strcmp(attr->attr.name, "ae_dark_cur_thresh") == 0)
        reg = AR013X_AD_AE_DARK_CUR_THRESH_REG;
    else
        dev_err(dev, "prucam: unknown show name %s", attr->attr.name);

    r = write_cam_reg(reg, input_value);
    if (r != 0)
        return r;

    return count;
}

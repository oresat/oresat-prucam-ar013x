#include <linux/delay.h>
#include <linux/gpio/consumer.h>

#include "cam_gpio.h"

struct gpio_desc *bus_oe, *flash, *cam_oe, *clk_en, *input_en, *reset, *saddr,
    *standby, *trigger, *vreg_en;

int init_cam_gpio(struct device *dev)
{
    int ret;

    bus_oe = devm_gpiod_get(dev, "bus-oe", GPIOD_OUT_HIGH);
    if (IS_ERR(bus_oe)) {
        ret = PTR_ERR(bus_oe);
        return ret;
    }

    cam_oe = devm_gpiod_get(dev, "cam-oe", GPIOD_OUT_HIGH);
    if (IS_ERR(cam_oe)) {
        ret = PTR_ERR(cam_oe);
        return ret;
    }

    clk_en = devm_gpiod_get(dev, "clk-en", GPIOD_OUT_LOW);
    if (IS_ERR(clk_en)) {
        ret = PTR_ERR(clk_en);
        return ret;
    }

    // enable input by default
    input_en = devm_gpiod_get(dev, "input-en", GPIOD_OUT_LOW);
    if (IS_ERR(input_en)) {
        ret = PTR_ERR(input_en);
        return ret;
    }

    flash = devm_gpiod_get(dev, "flash", GPIOD_OUT_HIGH);
    if (IS_ERR(flash)) {
        ret = PTR_ERR(flash);
        return ret;
    }

    reset = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(reset)) {
        ret = PTR_ERR(reset);
        return ret;
    }

    // sensor address select
    saddr = devm_gpiod_get(dev, "saddr", GPIOD_OUT_LOW);
    if (IS_ERR(saddr)) {
        ret = PTR_ERR(saddr);
        return ret;
    }

    standby = devm_gpiod_get(dev, "standby", GPIOD_OUT_LOW);
    if (IS_ERR(standby)) {
        ret = PTR_ERR(standby);
        return ret;
    }

    trigger = devm_gpiod_get(dev, "trigger", GPIOD_OUT_LOW);
    if (IS_ERR(trigger)) {
        ret = PTR_ERR(trigger);
        return ret;
    }

    vreg_en = devm_gpiod_get(dev, "vreg-en", GPIOD_OUT_LOW);
    if (IS_ERR(vreg_en)) {
        ret = PTR_ERR(vreg_en);
        return ret;
    }

    return 0;
}

void free_cam_gpio(struct device *dev)
{
    // device managed gpio are freeed when the device detaches
}

void camera_enable(void)
{
    // inputs should already be enabled, but do it here just to be sure
    gpiod_set_value(input_en, 0);

    // enable the voltage regulators
    gpiod_set_value(vreg_en, 1);

    // per datasheet, wait a bit after enabling power before enabling clock
    msleep(50);

    // enable clock
    gpiod_set_value(clk_en, 1);

    // wait a little before performing reset
    msleep(50);

    // assert reset
    gpiod_set_value(reset, 0);

    // wait a little before deasserting reset
    msleep(2);

    // deassert reset
    gpiod_set_value(reset, 1);

    // disable standby
    gpiod_set_value(standby, 0);

    // once everythign else is set, enable the bus outputs
    gpiod_set_value(cam_oe, 0);
    gpiod_set_value(bus_oe, 0);

    // AR0130 datasheet says sleep for a little bit after enabled vregs and
    // clock
    msleep(10);
}

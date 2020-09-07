"""
Unit test for camera.py
"""
import pytest
from prucam import Camera, CONTEXT_A, CONTEXT_B


c = Camera()


def test_read_sysfs():
    """ test reading all sysfs attributes. """
    try:
        temp = c.context
        temp = c.image_size
        temp = c.coarse_intergation_time
        temp = c.fine_intergation_time
        temp = c.y_odd_inc
        temp = c.green1_gain
        temp = c.blue_gain
        temp = c.red_gain
        temp = c.green2_gain
        temp = c.global_gain
        temp = c.analog_gain
        temp = c.frame_length_lines
        temp = c.digital_binning
    except Exception:
        assert False
    assert True


def test_write_sysfs():
    """ test writing all sysfs attributes. """
    try:
        c.context = CONTEXT_A
        c.context = CONTEXT_B
        c.image_size = (1000, 200)
        c.coarse_intergation_time = 60
        c.fine_intergation_time = 40
        c.y_odd_inc = 0
        c.green1_gain = 0
        c.blue_gain = 0
        c.red_gain = 0
        c.green2_gain = 0
        c.global_gain = 0
        c.analog_gain = 0
        c.frame_length_lines = 0
        c.digital_binning = 0
    except Exception:
        assert False
    assert True

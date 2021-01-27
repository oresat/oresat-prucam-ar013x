"""Unit test for camera.py"""

import pytest
from prucam import Camera, CONTEXT_A, CONTEXT_B


@pytest.fixture
def camera():
    """make the file_cache object"""
    return Camera()


def test_read_sysfs(camera):
    """ test reading all sysfs attributes. """

    temp = camera.context
    temp = camera.image_size
    temp = camera.coarse_intergation_time
    temp = camera.fine_intergation_time
    temp = camera.y_odd_inc
    temp = camera.green1_gain
    temp = camera.blue_gain
    temp = camera.red_gain
    temp = camera.green2_gain
    temp = camera.global_gain
    temp = camera.analog_gain
    temp = camera.frame_length_lines
    temp = camera.digital_binning
    assert True


def test_write_sysfs(camera):
    """ test writing all sysfs attributes. """

    camera.context = CONTEXT_A
    camera.context = CONTEXT_B
    camera.image_size = (1000, 200)
    camera.coarse_intergation_time = 60
    camera.fine_intergation_time = 40
    camera.y_odd_inc = 0
    camera.green1_gain = 0
    camera.blue_gain = 0
    camera.red_gain = 0
    camera.green2_gain = 0
    camera.global_gain = 0
    camera.analog_gain = 0
    camera.frame_length_lines = 0
    camera.digital_binning = 0
    assert True

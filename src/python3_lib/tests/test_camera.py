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
    temp = camera.coarse_integration_time
    temp = camera.fine_integration_time
    temp = camera.y_odd_inc
    temp = camera.green1_gain
    temp = camera.blue_gain
    temp = camera.red_gain
    temp = camera.green2_gain
    temp = camera.global_gain
    temp = camera.analog_gain
    temp = camera.frame_length_lines
    temp = camera.digital_binning

    temp = camera.ae_ag_en
    temp = camera.ae_ag_exposure_hi
    temp = camera.ae_ag_exposure_lo
    temp = camera.ae_damp_gain
    temp = camera.ae_damp_max
    temp = camera.ae_damp_offset
    temp = camera.ae_dark_cur_thresh
    temp = camera.ae_dg_en
    temp = camera.ae_enable
    temp = camera.ae_luma_target
    temp = camera.ae_max_ev_step
    temp = camera.ae_max_exposure
    temp = camera.ae_min_ana_gain
    temp = camera.ae_min_ev_step
    temp = camera.ae_min_exposure
    temp = camera.ae_roi_x_size
    temp = camera.ae_roi_x_start_offset
    temp = camera.ae_roi_y_size
    temp = camera.ae_roi_y_start_offset
    assert True


def test_write_sysfs(camera):
    """ test writing all sysfs attributes. """

    camera.context = CONTEXT_A
    camera.context = CONTEXT_B
    camera.image_size = (1000, 200)
    camera.coarse_integration_time = 60
    camera.fine_integration_time = 40
    camera.y_odd_inc = 0
    camera.green1_gain = 0
    camera.blue_gain = 0
    camera.red_gain = 0
    camera.green2_gain = 0
    camera.global_gain = 0
    camera.analog_gain = 0
    camera.frame_length_lines = 0
    camera.digital_binning = 0

    camera.ae_ag_en = 1
    camera.ae_ag_exposure_hi = 1234
    camera.ae_ag_exposure_lo = 1234
    camera.ae_damp_gain = 1234
    camera.ae_damp_max = 1234
    camera.ae_damp_offset = 1234
    camera.ae_dark_cur_thresh = 1234
    camera.ae_dg_en = 1
    camera.ae_enable = 1
    camera.ae_luma_target = 1234
    camera.ae_max_ev_step = 1234
    camera.ae_max_exposure = 1234
    camera.ae_min_ana_gain = 1234
    camera.ae_min_ev_step = 1234
    camera.ae_min_exposure = 1234
    camera.ae_roi_x_size = 1234
    camera.ae_roi_x_start_offset = 1234
    camera.ae_roi_y_size = 1234
    camera.ae_roi_y_start_offset = 1234
    assert True

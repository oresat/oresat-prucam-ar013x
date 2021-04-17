"""A interface for controlling the camera."""

from datetime import datetime
import os
import io
import numpy as np
import cv2

CONTEXT_A = 0
CONTEXT_B = 1

# sysfs settings are broken into 2 separate directories
_CTX_SETTINGS = "context_settings/"
_AE_SETTINGS = "auto_exposure_settings/"

# Filenames for camera control. Defined by prucam kernel module.
_CAPTURE = "capture"
_CONTEXT = _CTX_SETTINGS + "context"

# Filenames for camera settings. Defined by prucam kernel module.
_IMAGE_X_SIZE = _CTX_SETTINGS + "x_size"
_IMAGE_Y_SIZE = _CTX_SETTINGS + "y_size"
_COARSE_INT_TIME = _CTX_SETTINGS + "coarse_time"
_FINE_INT_TIME = _CTX_SETTINGS + "fine_time"
_Y_ODD_INC = _CTX_SETTINGS + "y_odd_inc"
_GREEN1_GAIN = _CTX_SETTINGS + "green1_gain"
_BLUE_GAIN = _CTX_SETTINGS + "blue_gain"
_RED_GAIN = _CTX_SETTINGS + "red_gain"
_GREEN2_GAIN = _CTX_SETTINGS + "green2_gain"
_GLOBAL_GAIN = _CTX_SETTINGS + "global_gain"
_ANALOG_GAIN = _CTX_SETTINGS + "analog_gain"
_FRAME_LEN_LINES = _CTX_SETTINGS + "frame_len_lines"
_DIGITAL_BINNING = _CTX_SETTINGS + "digital_binning"

_AE_AG_EN = _AE_SETTINGS + "ae_ag_en"
_AE_AG_EXPOSURE_HI = _AE_SETTINGS + "ae_ag_exposure_hi"
_AE_AG_EXPOSURE_LO = _AE_SETTINGS + "ae_ag_exposure_lo"
_AE_DAMP_GAIN = _AE_SETTINGS + "ae_damp_gain"
_AE_DAMP_MAX = _AE_SETTINGS + "ae_damp_max"
_AE_DAMP_OFFSET = _AE_SETTINGS + "ae_damp_offset"
_AE_DARK_CUR_THRESH = _AE_SETTINGS + "ae_dark_cur_thresh"
_AE_DG_EN = _AE_SETTINGS + "ae_dg_en"
_AE_ENABLE = _AE_SETTINGS + "ae_enable"
_AE_LUMA_TARGET = _AE_SETTINGS + "ae_luma_target"
_AE_MAX_EV_STEP = _AE_SETTINGS + "ae_max_ev_step"
_AE_MAX_EXPOSURE = _AE_SETTINGS + "ae_max_exposure"
_AE_MIN_ANA_GAIN = _AE_SETTINGS + "ae_min_ana_gain"
_AE_MIN_EV_STEP = _AE_SETTINGS + "ae_min_ev_step"
_AE_MIN_EXPOSURE = _AE_SETTINGS + "ae_min_exposure"
_AE_ROI_X_SIZE = _AE_SETTINGS + "ae_roi_x_size"
_AE_ROI_X_START_OFFSET = _AE_SETTINGS + "ae_roi_x_start_offset"
_AE_ROI_Y_SIZE = _AE_SETTINGS + "ae_roi_y_size"
_AE_ROI_Y_START_OFFSET = _AE_SETTINGS + "ae_roi_y_start_offset"



class Camera:
    """This class contains all camera functionality.

    Attributes
    ----------
    context: int
        AR0130CS has two contexts (two set of reqisters for camera setting) for
        it camera setting allow swapping between two differnt camera setup mode
        trivial.
    image_size: (int, int)
        Tuple for the image size (x_size, y_size).
    coarse_integration_time: int
        Coarse exposure time when taking a image.
    fine_integration_time: int
        Fine exposure time when taking a image.
    y_odd_inc: int
        For enabling subsampling in y.
    green1_gain: int
        Green1 (aka GreenB) gain when taking a image.
    blue_gain: int
        Blue gain when taking a image.
    red_gain: int
        Red gain when taking a image.
    green2_gain: int
        Green2 (aka GreenR) gain when taking a image.
    global_gain: int
        Global gain when taking a image.
    analog_gain: int
        Analog gain when taking a image.
    frame_length_lines: int
        Frame Length Lines.
    digital_binning: int
        For reducing the output resolution.

    ae_ag_en: bool
        When set, enables the automatic AE control of analog gain
    ae_ag_exposure_hi: int
        At this integration time, the analog gain is increased
    ae_ag_exposure_lo: int
        At this integration time, the AE is reduced
    ae_damp_gain: int
        Adjusts step size and settling speed.
    ae_damp_max: int
        Max value allowed for recursiveDamp
    ae_damp_offset: int
        Adjusts step size and settling speed.
    ae_dark_cur_thresh: int
        The dark current level that stops AE from increasing integration time.
    ae_dg_en: bool
        Automatic control of digital gain by AE
    ae_enable: bool
        Enables the on-chip AE algorithm
    ae_luma_target: int
        Average luma target value to be reached by the auto exposure
    ae_max_ev_step: int
        Minimum exposure value step size
    ae_max_exposure: int
        Maximum integration (exposure) time in rows to be used by AE.
    ae_min_ana_gain: 2 bits
        Minimum analog gain to be used by AE.
    ae_min_ev_step: int
        Minimum exposure value step size
    ae_min_exposure: int
        Minimum integration (exposure) time in rows to be used by AE.
    ae_roi_x_size: int
        Number of columns in the ROI
    ae_roi_x_start_offset: int
        Number of pixels into each row before the ROI starts
    ae_roi_y_size:
        Number of rows in the ROI
    ae_roi_y_start_offset:
        Number of rows into each frame before the ROI starts
    """

    def __init__(self):
        self._sysfs_path = "/sys/class/pru/prucam/"
        self._capture_path = "/dev/prucam"

        # camera settings sysfs filepaths
        self._sysfs_attr_files = [
            _CAPTURE,
            _CONTEXT,
            _IMAGE_X_SIZE,
            _IMAGE_Y_SIZE,
            _COARSE_INT_TIME,
            _FINE_INT_TIME,
            _Y_ODD_INC,
            _GREEN1_GAIN,
            _RED_GAIN,
            _BLUE_GAIN,
            _GREEN2_GAIN,
            _GLOBAL_GAIN,
            _ANALOG_GAIN,
            _FRAME_LEN_LINES,
            _DIGITAL_BINNING,

            _AE_AG_EN,
            _AE_AG_EXPOSURE_HI,
            _AE_AG_EXPOSURE_LO,
            _AE_DAMP_GAIN,
            _AE_DAMP_MAX,
            _AE_DAMP_OFFSET,
            _AE_DARK_CUR_THRESH,
            _AE_DG_EN,
            _AE_ENABLE,
            _AE_LUMA_TARGET,
            _AE_MAX_EV_STEP,
            _AE_MAX_EXPOSURE,
            _AE_MIN_ANA_GAIN,
            _AE_MIN_EV_STEP,
            _AE_MIN_EXPOSURE,
            _AE_ROI_X_SIZE,
            _AE_ROI_X_START_OFFSET,
            _AE_ROI_Y_SIZE,
            _AE_ROI_Y_START_OFFSET,
        ]

        self._cols = self._get_camera_setting(_IMAGE_X_SIZE)
        self._rows = self._get_camera_setting(_IMAGE_Y_SIZE)

    def _get_camera_setting(self, setting: str):
        """
        Gets the camera"s setting.

        attributes
        ----------
        setting: int
            The camera setting to get.

        returns
        -------
        int
            The value for the setting.

        FileExistsError
            Missing kernel module sysfs attributes.
        """

        value = 0

        if setting not in self._sysfs_attr_files:
            msg = "Unknown camera setting for getter: {}.".format(setting)
            raise FileExistsError(msg)

        try:
            with open(self._sysfs_path + setting, "r") as f:
                value = int(f.read())
        except Exception as exc:
            msg = "Error {} when trying to read from  {}.".format(
                exc,
                setting
            )
            raise FileExistsError(msg)

        return value

    def _set_camera_setting(self, setting: str, value: int):
        """
        Sets the camera"s setting.

        attributes
        ----------
        setting: int
            The camera setting to get.
        value: int
            The new value for the setting.

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """

        value_str = str(value)

        if setting not in self._sysfs_attr_files:
            msg = "Unknown camera setting for setter: {}.".format(setting)
            raise FileExistsError(msg)

        try:
            with open(self._sysfs_path + setting, "w") as f:
                f.write(value_str)
        except Exception as exc:
            msg = "Error {} when trying to write {} to {}.".format(
                exc,
                value_str,
                setting
            )
            raise FileExistsError(msg)

    def capture(self, dir_path="/tmp/", ext=".bmp", color=True):
        """Grab an image from the directory.

        Parameters
        ----------
        dir_path
            Path to save file to.

        Raises
        ------
        ValueError
            if ext is not valid

        Returns
        -------
        str
            A filepath to the new image
        """

        # Read raw data
        fd = os.open(self._capture_path, os.O_RDWR)
        fio = io.FileIO(fd, closefd = False)
        imgbuf = bytearray(self._rows * self._cols)
        fio.readinto(imgbuf)
        fio.close()
        os.close(fd)

        # Convert to image
        img = np.frombuffer(imgbuf, dtype=np.uint8).reshape(
            self._rows,
            self._cols
        )

        # Convert to color
        if color is True:
            img = cv2.cvtColor(img, cv2.COLOR_BayerBG2BGR)

        # try to encode the image with the provided image extension
        ok, encoded = cv2.imencode(ext, img)
        if not ok:
            raise ValueError("{} encode error".format(ext))

        # Save image
        img_path = dir_path + "capture-" + datetime.now().isoformat() + ext
        with open(img_path, "wb") as f:
            f.write(encoded)

        return img_path

    # ------------------------------------------------------------------------
    # getters and setters

    @property
    def context(self):
        """
        Select the context.

        :getter: Returns 0 for context A or 1 for context B.
        :setter: Sets context.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        AttributeError
            Not a valid context value when setting context, must be
            CONTEXT_A or CONTEXT_B.
        """
        return self._get_camera_setting(_CONTEXT)

    @context.setter
    def context(self, value: int):
        if value != CONTEXT_A and value != CONTEXT_B:
            raise AttributeError("unkown context value")
        return self._set_camera_setting(_CONTEXT, value)

    @property
    def image_size(self):
        """
        The images size. Note maxium size is 1280 x 960.

        :getter: Returns size of the image (columns, rows).
        :setter: Sets the image size.
        :type: (int, int)

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        AttributeError
            Not valid input. must be (rows, cols).
        """
        x_value = self._get_camera_setting(_IMAGE_X_SIZE)
        y_value = self._get_camera_setting(_IMAGE_Y_SIZE)
        return (x_value, y_value)

    @image_size.setter
    def image_size(self, size_values: (int)):
        if len(size_values) != 2:
            msg = "Error not two size values for image size."
            raise AttributeError(msg)

        self._set_camera_setting(_IMAGE_X_SIZE, size_values[0])
        self._set_camera_setting(_IMAGE_Y_SIZE, size_values[1])
        self._cols = size_values[0]
        self._rows = size_values[1]

    @property
    def coarse_integration_time(self):
        """
        The coarse integration time.

        :getter: Returns the coarse integration time.
        :setter: Sets the coarse integration time.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_COARSE_INT_TIME)

    @coarse_integration_time.setter
    def coarse_integration_time(self, value: int):
        return self._set_camera_setting(_COARSE_INT_TIME, value)

    @property
    def fine_integration_time(self):
        """
        The fine integration time.

        :getter: Returns the fine integration time.
        :setter: Sets the fine integration time.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_FINE_INT_TIME)

    @fine_integration_time.setter
    def fine_integration_time(self, value: int):
        return self._set_camera_setting(_FINE_INT_TIME, value)

    @property
    def y_odd_inc(self):
        """
        The y odd inc.

        :getter: Returns the y odd inc.
        :setter: Sets the y oddf inc.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_Y_ODD_INC)

    @y_odd_inc.setter
    def y_odd_inc(self, value: int):
        return self._set_camera_setting(_Y_ODD_INC, value)

    @property
    def green1_gain(self):
        """
        The green1 (greenR) gain.

        :getter: Returns the green1 gain.
        :setter: Sets the green1 gain.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_GREEN1_GAIN)

    @green1_gain.setter
    def green1_gain(self, value: int):
        return self._set_camera_setting(_GREEN1_GAIN, value)

    @property
    def blue_gain(self):
        """
        The blue gain.

        :getter: Returns the blue gain.
        :setter: Sets the blue gain.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_BLUE_GAIN)

    @blue_gain.setter
    def blue_gain(self, value: int):
        return self._set_camera_setting(_BLUE_GAIN, value)

    @property
    def red_gain(self):
        """
        The red gain.

        :getter: Returns the red gain.
        :setter: Sets the red gain.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_RED_GAIN)

    @red_gain.setter
    def red_gain(self, value: int):
        return self._set_camera_setting(_RED_GAIN, value)

    @property
    def green2_gain(self):
        """
        The green2 (greenB) gain.

        :getter: Returns the green2 gain.
        :setter: Sets the green2 gain.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_GREEN2_GAIN)

    @green2_gain.setter
    def green2_gain(self, value: int):
        return self._set_camera_setting(_GREEN2_GAIN, value)

    @property
    def global_gain(self):
        """
        The global gain.

        :getter: Returns the global gain.
        :setter: Sets the global gain.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_GLOBAL_GAIN)

    @global_gain.setter
    def global_gain(self, value: int):
        return self._set_camera_setting(_GLOBAL_GAIN, value)

    @property
    def analog_gain(self):
        """
        The analog gain.

        :getter: Returns the analog gain.
        :setter: Sets the analog gain.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_ANALOG_GAIN)

    @analog_gain.setter
    def analog_gain(self, value: int):
        return self._set_camera_setting(_ANALOG_GAIN, value)

    @property
    def frame_length_lines(self):
        """
        The frame length lines.

        :getter: Returns the analog gain.
        :setter: Sets the analog gain.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_FRAME_LEN_LINES)

    @frame_length_lines.setter
    def frame_length_lines(self, value: int):
        return self._set_camera_setting(_FRAME_LEN_LINES, value)

    @property
    def digital_binning(self):
        """
        The digital binning.

        :getter: Returns the digital binning.
        :setter: Sets the digital binning.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_DIGITAL_BINNING)

    @digital_binning.setter
    def digital_binning(self, value: int):
        return self._set_camera_setting(_DIGITAL_BINNING, value)

    @property
    def ae_ag_en(self):
        """
        The ae ag en.

        :getter: Returns the ae ag en.
        :setter: Sets the ae ag en.
        :type: bool

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_AG_EN)

    @ae_ag_en.setter
    def ae_ag_en(self, value: int):
        return self._set_camera_setting(_AE_AG_EN, value)

    @property
    def ae_ag_exposure_hi(self):
        """
        The ae ag exposure hi.

        :getter: Returns the ae ag exposure hi.
        :setter: Sets the ae ag exposure hi.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_AG_EXPOSURE_HI)

    @ae_ag_exposure_hi.setter
    def ae_ag_exposure_hi(self, value: int):
        return self._set_camera_setting(_AE_AG_EXPOSURE_HI, value)

    @property
    def ae_ag_exposure_lo(self):
        """
        The ae ag exposure lo.

        :getter: Returns the ae ag exposure lo.
        :setter: Sets the ae ag exposure lo.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_AG_EXPOSURE_LO)

    @ae_ag_exposure_lo.setter
    def ae_ag_exposure_lo(self, value: int):
        return self._set_camera_setting(_AE_AG_EXPOSURE_LO, value)

    @property
    def ae_damp_gain(self):
        """
        The ae damp gain.

        :getter: Returns the ae damp gain.
        :setter: Sets the ae damp gain.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_DAMP_GAIN)

    @ae_damp_gain.setter
    def ae_damp_gain(self, value: int):
        return self._set_camera_setting(_AE_DAMP_GAIN, value)

    @property
    def ae_damp_max(self):
        """
        The ae damp max.

        :getter: Returns the ae damp max.
        :setter: Sets the ae damp max.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_DAMP_MAX)

    @ae_damp_max.setter
    def ae_damp_max(self, value: int):
        return self._set_camera_setting(_AE_DAMP_MAX, value)

    @property
    def ae_damp_offset(self):
        """
        The ae damp offset.

        :getter: Returns the ae damp offset.
        :setter: Sets the ae damp offset.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_DAMP_OFFSET)

    @ae_damp_offset.setter
    def ae_damp_offset(self, value: int):
        return self._set_camera_setting(_AE_DAMP_OFFSET, value)

    @property
    def ae_dark_cur_thresh(self):
        """
        The ae dark cur thresh.

        :getter: Returns the ae dark cur thresh.
        :setter: Sets the ae dark cur thresh.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_DARK_CUR_THRESH)

    @ae_dark_cur_thresh.setter
    def ae_dark_cur_thresh(self, value: int):
        return self._set_camera_setting(_AE_DARK_CUR_THRESH, value)

    @property
    def ae_dg_en(self):
        """
        The ae dg en.

        :getter: Returns the ae dg en.
        :setter: Sets the ae dg en.
        :type: bool

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_DG_EN)

    @ae_dg_en.setter
    def ae_dg_en(self, value: int):
        return self._set_camera_setting(_AE_DG_EN, value)

    @property
    def ae_enable(self):
        """
        The ae enable.

        :getter: Returns the ae enable.
        :setter: Sets the ae enable.
        :type: bool

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_ENABLE)

    @ae_enable.setter
    def ae_enable(self, value: int):
        return self._set_camera_setting(_AE_ENABLE, value)

    @property
    def ae_luma_target(self):
        """
        The ae luma target.

        :getter: Returns the ae luma target.
        :setter: Sets the ae luma target.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_LUMA_TARGET)

    @ae_luma_target.setter
    def ae_luma_target(self, value: int):
        return self._set_camera_setting(_AE_LUMA_TARGET, value)

    @property
    def ae_max_ev_step(self):
        """
        The ae max ev step.

        :getter: Returns the ae max ev step.
        :setter: Sets the ae max ev step.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_MAX_EV_STEP)

    @ae_max_ev_step.setter
    def ae_max_ev_step(self, value: int):
        return self._set_camera_setting(_AE_MAX_EV_STEP, value)

    @property
    def ae_max_exposure(self):
        """
        The ae max exposure.

        :getter: Returns the ae max exposure.
        :setter: Sets the ae max exposure.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_MAX_EXPOSURE)

    @ae_max_exposure.setter
    def ae_max_exposure(self, value: int):
        return self._set_camera_setting(_AE_MAX_EXPOSURE, value)

    @property
    def ae_min_ana_gain(self):
        """
        The ae min ana gain.

        :getter: Returns the ae min ana gain.
        :setter: Sets the ae min ana gain.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_MIN_ANA_GAIN)

    @ae_min_ana_gain.setter
    def ae_min_ana_gain(self, value: int):
        return self._set_camera_setting(_AE_MIN_ANA_GAIN, value)

    @property
    def ae_min_ev_step(self):
        """
        The ae min ev step.

        :getter: Returns the ae min ev step.
        :setter: Sets the ae min ev step.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_MIN_EV_STEP)

    @ae_min_ev_step.setter
    def ae_min_ev_step(self, value: int):
        return self._set_camera_setting(_AE_MIN_EV_STEP, value)

    @property
    def ae_min_exposure(self):
        """
        The ae min exposure.

        :getter: Returns the ae min exposure.
        :setter: Sets the ae min exposure.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_MIN_EXPOSURE)

    @ae_min_exposure.setter
    def ae_min_exposure(self, value: int):
        return self._set_camera_setting(_AE_MIN_EXPOSURE, value)

    @property
    def ae_roi_x_size(self):
        """
        The ae roi x size.

        :getter: Returns the ae roi x size.
        :setter: Sets the ae roi x size.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_ROI_X_SIZE)

    @ae_roi_x_size.setter
    def ae_roi_x_size(self, value: int):
        return self._set_camera_setting(_AE_ROI_X_SIZE, value)

    @property
    def ae_roi_x_start_offset(self):
        """
        The ae roi x start offset.

        :getter: Returns the ae roi x start offset.
        :setter: Sets the ae roi x start offset.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_ROI_X_START_OFFSET)

    @ae_roi_x_start_offset.setter
    def ae_roi_x_start_offset(self, value: int):
        return self._set_camera_setting(_AE_ROI_X_START_OFFSET, value)

    @property
    def ae_roi_y_size(self):
        """
        The ae roi y size.

        :getter: Returns the ae roi y size.
        :setter: Sets the ae roi y size.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_ROI_Y_SIZE)

    @ae_roi_y_size.setter
    def ae_roi_y_size(self, value: int):
        return self._set_camera_setting(_AE_ROI_Y_SIZE, value)

    @property
    def ae_roi_y_start_offset(self):
        """
        The ae roi y start offset.

        :getter: Returns the ae roi y start offset.
        :setter: Sets the ae roi y start offset.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_AE_ROI_Y_START_OFFSET)

    @ae_roi_y_start_offset.setter
    def ae_roi_y_start_offset(self, value: int):
        return self._set_camera_setting(_AE_ROI_Y_START_OFFSET, value)

"""
PRUcam interface
================

The core of the camera module, located in src/camera.
"""

from datetime import datetime
from PIL import Image


CONTEXT_A = 0
CONTEXT_B = 1


# Filenames for camera control. Defined by prucam kernel module.
_CAPTURE = "capture"
_CONTEXT = "context_select"


# Filenames for camera settings. Defined by prucam kernel module.
_IMAGE_X_SIZE = "image_x_size"
_IMAGE_Y_SIZE = "image_y_size"
_COARSE_INT_TIME = "coarse_int_time"
_FINE_INT_TIME = "fine_int_time"
_Y_ODD_INC = "y_odd_inc"
_GREEN1_GAIN = "green1_gain"
_BLUE_GAIN = "blue_gain"
_RED_GAIN = "red_gain"
_GREEN2_GAIN = "green2_gain"
_GLOBAL_GAIN = "global_gain"
_ANALOG_GAIN = "analog_gain"
_FRAME_LEN_LINES = "frame_len_lines"
_DIGITAL_BINNING = "digital_binning"


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
    coarse_intergation_time: int
        Coarse exposure time when taking a image.
    fine_intergation_time: int
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

    """

    def __init__(self, pru=0):
        """
        Parameters
        ----------
        pru: int
            Which pru to use. Should be 0 or 1. Default is 0.
        """

        self._pru = pru

        self._sysfs_path = "/sys/kernel/prucam/"
        self._capture_path="/dev/prucam"

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
            ]

    def _get_camera_setting(self, setting: str):
        """
        Gets the camera's setting.

        attributes
        ----------
        setting: int
            The camera setting to get.

        returns
        -------
        int
            The value for the setting.
        """

        value = 0

        if setting not in self._sysfs_attr_files:
            msg = "Unknown camera setting for getter: {}.".format(setting)
            raise Exception(msg)

        try:
            with open(self._sysfs_path + setting, 'r') as f:
                value = int(f.read())
        except Exception as e:
            print(e)  # TODO log this
            msg = "Error when trying to read from {}.".format(setting)
            raise Exception(msg)

        return value

    def _set_camera_setting(self, setting: str, value: int):
        """
        Sets the camera's setting.

        attributes
        ----------
        setting: int
            The camera setting to get.
        value: int
            The new value for the setting.
        """

        value_str = str(value)

        if setting not in self._sysfs_attr_files:
            msg = "Unknown camera setting for setter: {}.".format(setting)
            raise Exception(msg)

        try:
            with open(self._sysfs_path + setting, 'w') as f:
                f.write(value_str)
        except Exception as e:
            msg = "Error {} when trying to write {} to {}.".format(
                e,
                value_str,
                setting
                )
            raise Exception(msg)

    def capture(self, path: str):
        """Grab an image from the directory.

        Returns
        -------
        OpenCV image array
            A random image from the currently-chosen directory.

        """

        if path[0] != '/' or path[-1] != '/':
            raise Exception("invalid path")

        now_str = datetime.now().isoformat()
        new_img_path = path + "capture-" + now_str + ".bmp"

        with open(self._capture_path, "rb") as f:
            img_raw = f.read()

        img = Image.frombytes("L", self.image_size, bytes(img_raw))

        with open(new_img_path, "w") as f:
            f.write(img)

    # ------------------------------------------------------------------------
    # getter

    @property
    def context(self):
        return self._get_camera_setting(_CONTEXT)

    @property
    def image_size(self):
        x_value = self._get_camera_setting(_IMAGE_X_SIZE)
        y_value = self._get_camera_setting(_IMAGE_Y_SIZE)
        return (x_value, y_value)

    @property
    def coarse_intergation_time(self):
        return self._get_camera_setting(_COARSE_INT_TIME)

    @property
    def fine_intergation_time(self):
        return self._get_camera_setting(_FINE_INT_TIME)

    @property
    def y_odd_inc(self):
        return self._get_camera_setting(_Y_ODD_INC)

    @property
    def green1_gain(self):
        return self._get_camera_setting(_GREEN1_GAIN)

    @property
    def blue_gain(self):
        return self._get_camera_setting(_BLUE_GAIN)

    @property
    def red_gain(self):
        return self._get_camera_setting(_RED_GAIN)

    @property
    def green2_gain(self):
        return self._get_camera_setting(_GREEN2_GAIN)

    @property
    def global_gain(self):
        return self._get_camera_setting(_GLOBAL_GAIN)

    @property
    def analog_gain(self):
        return self._get_camera_setting(_ANALOG_GAIN)

    @property
    def frame_length_lines(self):
        return self._get_camera_setting(_FRAME_LEN_LINES)

    @property
    def digital_binning(self):
        return self._get_camera_setting(_DIGITAL_BINNING)

    # ------------------------------------------------------------------------
    # setter

    @context.setter
    def context(self, value: int):
        return self._set_camera_setting(_CONTEXT, value)

    @image_size.setter
    def image_size(self, size_values: (int)):
        if len(size_values) != 2:
            msg = "Error not two size values for image size."
            raise Exception(msg)

        self._set_camera_setting(_IMAGE_X_SIZE, size_values[0])
        self._set_camera_setting(_IMAGE_Y_SIZE, size_values[1])

    @coarse_intergation_time.setter
    def coarse_intergation_time(self, value: int):
        return self._set_camera_setting(_COARSE_INT_TIME, value)

    @fine_intergation_time.setter
    def fine_intergation_time(self, value: int):
        return self._set_camera_setting(_FINE_INT_TIME, value)

    @y_odd_inc.setter
    def y_odd_inc(self, value: int):
        return self._set_camera_setting(_Y_ODD_INC, value)

    @green1_gain.setter
    def green1_gain(self, value: int):
        return self._set_camera_setting(_GREEN1_GAIN, value)

    @blue_gain.setter
    def blue_gain(self, value: int):
        return self._set_camera_setting(_BLUE_GAIN, value)

    @red_gain.setter
    def red_gain(self, value: int):
        return self._set_camera_setting(_RED_GAIN, value)

    @green2_gain.setter
    def green2_gain(self, value: int):
        return self._set_camera_setting(_GREEN2_GAIN, value)

    @global_gain.setter
    def global_gain(self, value: int):
        return self._set_camera_setting(_GLOBAL_GAIN, value)

    @analog_gain.setter
    def analog_gain(self, value: int):
        return self._set_camera_setting(_ANALOG_GAIN, value)

    @frame_length_lines.setter
    def frame_length_lines(self, value: int):
        return self._set_camera_setting(_FRAME_LEN_LINES, value)

    @digital_binning.setter
    def digital_binning(self, value: int):
        return self._set_camera_setting(_DIGITAL_BINNING, value)

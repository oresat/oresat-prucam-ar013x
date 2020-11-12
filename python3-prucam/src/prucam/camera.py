"""A interface for controlling the camera."""

from datetime import datetime
from PIL import Image


CONTEXT_A = 0
CONTEXT_B = 1


# Filenames for camera control. Defined by prucam kernel module.
_CAPTURE = "capture"
_CONTEXT = "context"


# Filenames for camera settings. Defined by prucam kernel module.
_IMAGE_X_SIZE = "x_size"
_IMAGE_Y_SIZE = "y_size"
_COARSE_INT_TIME = "coarse_time"
_FINE_INT_TIME = "fine_time"
_Y_ODD_INC = "y_odd_inc"
_GREEN1_GAIN = "green1_gain"
_BLUE_GAIN = "blue_gain"
_RED_GAIN = "red_gain"
_GREEN2_GAIN = "green2_gain"
_GLOBAL_GAIN = "global_gain"
_ANALOG_GAIN = "analog_gain"
_FRAME_LEN_LINES = "frame_len_lines"
_DIGITAL_BINNING = "digital_binding"


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

        FileExistsError
            Missing kernel module sysfs attributes.
        """

        value = 0

        if setting not in self._sysfs_attr_files:
            msg = "Unknown camera setting for getter: {}.".format(setting)
            raise FileExistsError(msg)

        try:
            with open(self._sysfs_path + setting, 'r') as f:
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
        Sets the camera's setting.

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
            with open(self._sysfs_path + setting, 'w') as f:
                f.write(value_str)
        except Exception as exc:
            msg = "Error {} when trying to write {} to {}.".format(
                exc,
                value_str,
                setting
                )
            raise FileExistsError(msg)

    def capture(self):
        """Grab an image from the directory.

        Returns
        -------
        image array
            A random image from the currently-chosen directory.

        """

        with open(self._capture_path, "rb") as f:
            img_raw = f.read()

        return img_raw

    def capture_and_save(self, path="./"):
        """Grab an image from pru and save it as a bmp.

        Attributes
        ----------
        path: str
            Path to save new image to.
        """

        if path[-1] != '/':
            path.apend('/')

        now_str = datetime.now().isoformat()
        new_img_path = path + "capture-" + now_str + ".bmp"

        img_raw = self.capture()

        img = Image.frombytes("L", self.image_size, bytes(img_raw))

        with open(new_img_path, "w") as f:
            f.write(img)

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
        if value != CONTEXT_A or value != CONTEXT_B:
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

    @property
    def coarse_intergation_time(self):
        """
        The coarse intergation time.

        :getter: Returns the coarse intergation time.
        :setter: Sets the coarse intergation time.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_COARSE_INT_TIME)

    @coarse_intergation_time.setter
    def coarse_intergation_time(self, value: int):
        return self._set_camera_setting(_COARSE_INT_TIME, value)

    @property
    def fine_intergation_time(self):
        """
        The fine intergation time.

        :getter: Returns the fine intergation time.
        :setter: Sets the fine intergation time.
        :type: int

        raise
        -----
        FileExistsError
            Missing kernel module sysfs attributes.
        """
        return self._get_camera_setting(_FINE_INT_TIME)

    @fine_intergation_time.setter
    def fine_intergation_time(self, value: int):
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

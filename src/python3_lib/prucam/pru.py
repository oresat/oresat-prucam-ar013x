"""An interface for controlling the image capture firmware running on the PRUs.
"""

from pathlib import Path


class PRUCam:
    """
    An interface for controlling the image capture firmware running on PRU0 and
    PRU1 (remoteproc1 and remoteproc2, respectively).
    """

    def __init__(self):
        self._pru0_path = "/sys/class/remoteproc/remoteproc1/"
        self._pru1_path = "/sys/class/remoteproc/remoteproc2/"
        self._pru0_fw = "prucam_pru0_fw.out"
        self._pru1_fw = "prucam_pru1_fw.out"

    def start(self):
        """
        Starts the PRUs. PRU1 is started before PRU0 because it is the master.

        Raises
        ------
        FileNotFoundError
            If remoteproc1 or remoteproc2 is not found. Most likely the
            device tree overlay for REMOTEPROC is not loaded.
        """

        self._check_prus_present()

        with open(self._pru1_path + "state", "w") as fptr:
            fptr.write("start")
        with open(self._pru0_path + "state", "w") as fptr:
            fptr.write("start")

    def stop(self):
        """
        Stops the PRUs. Order does not matter for now.

        Raises
        ------
        FileNotFoundError
            If remoteproc1 or remoteproc2 is not found. Most likely the device
            tree overlay for REMOTEPROC is not loaded.
        """

        self._check_prus_present()

        with open(self._pru0_path + "state", "w") as fptr:
            fptr.write("stop")
        with open(self._pru1_path + "state", "w") as fptr:
            fptr.write("stop")

    def restart(self):
        """
        Restarts the PRUs.
        """

        self.stop()
        self.start()

    def status(self):
        """
        Get status of pru.

        Returns
        -------
        str
            The PRU0 state
        str
            The PRU1 state

        Raises
        ------
        FileNotFoundError
            If remoteproc1 or remoteproc2 is not found. Most likely the device
            tree overlay for REMOTEPROC is not loaded.
        """

        self._check_prus_present()

        with open(self._pru0_path + "state", "r") as fptr:
            pru0_status = fptr.read()
        with open(self._pru1_path + "state", "r") as fptr:
            pru1_status = fptr.read()

        return pru0_status, pru1_status

    def _load_fw(self):
        """
        Load the firmware onto the pru.

        Raises
        ------
        FileNotFoundError
            If remoteproc1 or remoteproc2 is not found. Most likely the device
            tree overlay for REMOTEPROC is not loaded.
        """

        self._check_prus_present()

        # check the FW files are present in /lib/firmware
        if not Path("/lib/firmware/" + self._pru0_fw).exists():
            msg = "{} is missing".format("/lib/firmware/" + self._pru0_fw)
            raise FileNotFoundError(msg)

        if not Path("/lib/firmware/" + self._pru1_fw).exists():
            msg = "{} is missing".format("/lib/firmware/" + self._pru1_fw)
            raise FileNotFoundError(msg)

        # write the name of the FW file in /lib/firmware to
        # /sys/class/remoteproc/remoteprocX/firmware
        with open(self._pru0_path + "firmware", "w") as fptr:
            fptr.write(self._pru0_fw)
        with open(self._pru1_path + "firmware", "w") as fptr:
            fptr.write(self._pru1_fw)

    def _check_prus_present(self):
        """
        Check that the remoteproc1 and remoteproc2 sysfs directories are
        present.

        Raises
        ------
        FileNotFoundError
            If remoteproc1 or remoteproc2 is not found. Most likely the device
            tree overlay for REMOTEPROC is not loaded.
        """

        if not Path(self._pru0_path).is_dir():
            raise FileNotFoundError("{} is missing".format(self._pru0_path))

        if not Path(self._pru1_path).is_dir():
            raise FileNotFoundError("{} is missing".format(self._pru1_path))

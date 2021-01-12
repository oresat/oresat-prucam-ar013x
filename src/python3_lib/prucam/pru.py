"""An interface for controlling the image capture firmware running on the PRUs."""

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

        with open(self._pru1_path + "status", 'w') as f:
            f.write('start')
        with open(self._pru0_path + "status", 'w') as f:
            f.write('start')

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

        with open(self._pru0_path + "status", 'w') as f:
            f.write('stop')
        with open(self._pru1_path + "status", 'w') as f:
            f.write('stop')

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
            The combined status of the PRUs

        Raises
        ------
        FileNotFoundError
            If remoteproc1 or remoteproc2 is not found. Most likely the device 
            tree overlay for REMOTEPROC is not loaded.
        """

        self._check_prus_present()

        with open(self._pru0_path + "status", 'r') as f:
            pru0_status = f.read()
        with open(self._pru1_path + "status", 'r') as f:
            pru1_status = f.read()

        return "PRU0={}; PRU1={}".format(pru0_status, pru1_status)

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
            raise FileNotFoundError("{} is missing".format("/lib/firmware/" + self._pru0_fw))
        elif not Path("/lib/firmware/" + self._pru1_fw).exists():
            raise FileNotFoundError("{} is missing".format("/lib/firmware/" + self._pru1_fw))

        # write the name of the FW file in /lib/firmware to
        # /sys/class/remoteproc/remoteprocX/firmware
        with open(self._pru0_path + "firmware", 'w') as f:
            f.write(self._pru0_fw)
        with open(self._pru1_path + "firmware", 'w') as f:
            f.write(self._pru1_fw)

    
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
        elif not Path(self._pru1_path).is_dir():
            raise FileNotFoundError("{} is missing".format(self._pru1_path))


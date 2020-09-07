"""A interface for controlling the pru."""

from pathlib import Path


class PRU:
    """
    A interface for controlling the pru.
    """

    def __init__(self):
        self._path = "/sys/class/remoteproc/remoteproc1/"

    def start(self):
        """
        Turn on the pru.

        Raises
        ------
        FileNotFoundError
            If remoteproc1 is not found. Most likely the device tree overaly
            for REMOTEPROC is not loaded.
        """
        if Path(self._path).is_dir():
            with open(self._path + "status", 'w') as f:
                f.write('start')
        else:
            raise FileNotFoundError("{} is missing".format(self._path))

    def stop(self):
        """
        Turn off the pru.

        Raises
        ------
        FileNotFoundError
            If remoteproc1 is not found. Most likely the device tree overaly
            for REMOTEPROC is not loaded.
        """

        if Path(self._path).is_dir():
            with open(self._path + "status", 'w') as f:
                f.write('stop')
        else:
            raise FileNotFoundError("{} is missing".format(self._path))

    def restart(self):
        """
        Restarts the pru.
        """

        self.stop()
        self.start()

    def status(self):
        """
        Get status of pru.

        Returns
        -------
        str
            The status of the pru.

        Raises
        ------
        FileNotFoundError
            If remoteproc1 is not found. Most likely the device tree overaly
            for REMOTEPROC is not loaded.
        """

        if Path(self._path).is_dir():
            with open(self._path + "status", 'r') as f:
                status = f.read()
        else:
            raise FileNotFoundError("{} is missing".format(self._path))

        return status

    def _load_fw(self):
        """
        Load the firmware onto the pru.

        Raises
        ------
        FileNotFoundError
            If remoteproc1 is not found. Most likely the device tree overaly
            for REMOTEPROC is not loaded.
        """

        if Path(self._path).is_dir():
            with open("fw/am335x-pru0-fw", 'r') as f:
                pru_fw = f.read()

            with open(self._path + "firmware", 'w') as f:
                f.write(pru_fw)
        else:
            raise FileNotFoundError("{} is missing".format(self._path))

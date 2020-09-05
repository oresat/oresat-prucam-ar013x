""" A interface for controlling the pru. """


class PRU:
    """
    A interface for controlling the pru.

    Attributes
    ----------
    pru: int
        Which PRU. Can be 0 or 1.
    """

    def __init__(self, pru=0):
        self._pru = pru

        if pru == 0:
            self._path = "/sys/class/remoteproc/remoteproc1/"
        elif pru == 1:
            self._path = "/sys/class/remoteproc/remoteproc2/"
        else:
            raise Exception("pru must be 0 or 1")

    def start(self):
        """
        Turn on the pru.
        """

        with open(self._path + "status", 'w') as f:
            f.write('start')

    def stop(self):
        """
        Turn off the pru.
        """

        with open(self._path + "status", 'w') as f:
            f.write('stop')

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
        """

        with open(self._path + "status", 'r') as f:
            status = f.read()

        return status

    def _load_fw(self):
        """
        Load the firmware onto the pru.
        """

        with open("fw/am335x-pru0-fw", 'r') as f:
            pru_fw = f.read()

        with open(self._path + "firmware", 'w') as f:
            f.write(pru_fw)

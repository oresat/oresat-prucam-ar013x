"""Unittest for PRU class"""

import pytest
from prucam import PRUCam

pru = PRUCam()


@pytest.fixture
def prucam():
    """make the PRUCam obj and  test loading the pru firmware onto the pru"""

    return PRUCam()


def test_start(prucam):
    """test staring the pru"""

    prucam.start()
    assert True


def test_stop(prucam):
    """test stoping the pru"""

    prucam.stop()
    assert True


def test_restart(prucam):
    """test restarting the pru"""

    # turn pru back on for test
    prucam.start()

    prucam.restart()
    assert True


def test_status(prucam):
    """test getting the state of the pru"""

    prucam.status()
    assert True

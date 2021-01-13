"""Unittest for PRU class"""

from prucam import PRUCam

pru = PRUCam()


def test_start():
    """test staring the pru"""
    try:
        pru.start()
    except Exception as exc:
        print(exc)
        assert False
    assert True


def test_stop():
    """test stoping the pru"""
    try:
        pru.stop()
    except Exception as exc:
        print(exc)
        assert False
    assert True


def test_restart():
    """test restarting the pru"""

    # turn pru back on for test
    pru.start()

    try:
        pru.restart()
    except Exception as exc:
        print(exc)
        assert False
    assert True


def test_state():
    """test getting the state of the pru"""
    try:
        pru.state()
    except Exception as exc:
        print(exc)
        assert False
    assert True


def test_load_fw():
    """test loading the pru firmware onto the pru"""

    pru.stop()

    try:
        pru._load_fw()
    except Exception as exc:
        print(exc)
        assert False
    assert True


"""Unittest for PRU class"""

from prucam import PRU

pru = PRU()


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
        pru.start()
    except Exception as exc:
        print(exc)
        assert False
    assert True


def test_restart():
    """test restarting the pru"""
    try:
        pru.restart()
    except Exception as exc:
        print(exc)
        assert False
    assert True


def test_status():
    """test getting the status of the pru"""
    try:
        pru.status()
    except Exception as exc:
        print(exc)
        assert False
    assert True


def test_load_fw():
    """test loading the pru firmware onto the pru"""
    try:
        pru._load_fw()
    except Exception as exc:
        print(exc)
        assert False
    assert True


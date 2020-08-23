# prucam-test-server

## Install

- install systemd service file
NOTE: make sure path in service file matches your system

```
$ sudo cp prucam-test-server.service /etc/systemd/system
```

- enable and start service

```
$ sudo systemctl enable prucam-test-server
$ sudo systemctl restart prucam-test-server

- Optional: reboot and see that service starts at boot

- Open a web browser on the host machine and navigate to 
  http://192.168.7.2:5000/testimage.jpg
  NOTE: filename can be anything
  NOTE: server will format image based on extension. 
  jpg, bmp, png, and others are supported

- Or, use `curl`

```
$ curl -O http://192.168.7.2:5000/testimage.jpg
```


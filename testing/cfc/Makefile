install:
	cp cfc-watchdog.service /etc/systemd/system
	cp cfc-server.service /etc/systemd/system
	systemctl daemon-reload
	systemctl enable cfc-server.service
	systemctl enable cfc-watchdog.service
	systemctl restart cfc-server.service
	systemctl restart cfc-watchdog.service

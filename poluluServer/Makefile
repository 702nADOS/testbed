all:
	g++ polulu.cpp GenericPacket.cpp servocontroller.cpp -o servocontroller





# ================================================================
# VDE setup. Do once per system session. DHCP is optional.
vde: vde-stop
	@vde_switch -d -s /tmp/switch1 -M /tmp/mgmt
	@sudo sysctl -w net.ipv4.ip_forward=1
	#@sudo /etc/init.d/networking restart
	@sudo vde_tunctl -u $(USER) -t tap0
	@sudo ifconfig tap0 10.0.3.1/24 up
	@sudo route add -net 10.0.3.0/24 dev tap0
	@sudo vde_tunctl -u $(USER) -t tap1
	@sudo ifconfig tap1 10.0.3.55/24 up
	@sudo /etc/init.d/isc-dhcp-server start
	@vde_plug2tap --daemon -s /tmp/switch1 tap0
	@vde_plug2tap --daemon -s /tmp/switch1 tap1

vde-stop:
	@-pkill vde_switch
	@-sudo vde_tunctl -d tap0
	@-sudo sysctl -w net.ipv4.ip_forward=0
	@-sudo /etc/init.d/isc-dhcp-server stop
	#@-sudo /etc/init.d/networking restart
	@-rm -rf /tmp/switch1

dhcp: dhcp-stop
	@slirpvde -d -s /tmp/switch1 -dhcp

dhcp-stop:
	@-pkill slirpvde

# Cleanup network shenanigans.
clean-network: dhcp-stop vde-stop
#
# ================================================================

# Redundant Failover Seemless IP Stack (RFSIS)
This document loosely describes how to install and operate our demo TCP Echo application for RFSIS.


## Instructions
These instruction describe how to compile and use the TCP Echo Application using RFSIS.

1. `git clone https://github.com/zppratt/rfsis.git`
2. `cd rfsis/lib/picotcp`
3. `make TAP=1`
4. `cd ../../`
5. `make`
7. `sudo ./echoserver`

### Note:
1. The tap device and the IP configuration in config.json need to be different.
2. If the echo server isn't working check the network configuration.

#### Network Configuration check
Please run `ifconfig`, it should look something like this:

```bash
br0       Link encap:Ethernet  HWaddr 5a:ec:f6:97:3d:f2
          inet addr:192.168.1.102  Bcast:192.168.1.255  Mask:255.255.255.0
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:116462 errors:0 dropped:0 overruns:0 frame:0
          TX packets:67984 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:6035386 (5.7 MiB)  TX bytes:5077951 (4.8 MiB)

eth0      Link encap:Ethernet  HWaddr b8:27:eb:3e:3f:de
          inet6 addr: fe80::b97:2990:8249:4308/64 Scope:Link
          UP BROADCAST RUNNING PROMISC MULTICAST  MTU:1500  Metric:1
          RX packets:141193 errors:0 dropped:0 overruns:0 frame:0
          TX packets:70064 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:16591394 (15.8 MiB)  TX bytes:6117672 (5.8 MiB)

lo        Link encap:Local Loopback
          inet addr:127.0.0.1  Mask:255.0.0.0
          inet6 addr: ::1/128 Scope:Host
          UP LOOPBACK RUNNING  MTU:65536  Metric:1
          RX packets:389 errors:0 dropped:0 overruns:0 frame:0
          TX packets:389 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1
          RX bytes:445806 (435.3 KiB)  TX bytes:445806 (435.3 KiB)

tap0      Link encap:Ethernet  HWaddr 5a:ec:f6:97:3d:f2
          inet addr:192.168.1.100  Bcast:192.168.1.255  Mask:255.255.255.0
          UP BROADCAST PROMISC MULTICAST  MTU:1500  Metric:1
          RX packets:9 errors:0 dropped:0 overruns:0 frame:0
          TX packets:91 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:625 (625.0 B)  TX bytes:11253 (10.9 KiB)

wlan0     Link encap:Ethernet  HWaddr b8:27:eb:6b:6a:8b
          inet addr:192.168.1.15  Bcast:192.168.1.255  Mask:255.255.255.0
          inet6 addr: fe80::a73f:6635:11bc:e976/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:94353 errors:0 dropped:41992 overruns:0 frame:0
          TX packets:3840 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:14242747 (13.5 MiB)  TX bytes:267728 (261.4 KiB)
```

1. If this is not what your getting, reboot the pi by `sudo reboot`.
2. `cd rfsis`
3. `sudo ./bridge.sh`
4. Run `ifconfig` again to verify the network is up

## Usage
1. Because I don't have the application port forwarded on my router yet, you can not test on your local machine with our development VMs.
2. You can install the project to your own VM instance, and then test using your local box, because the VM should see the LAN.
3. Or just ssh into one of the VM's on my LAN, and test the echo server from there.
4. To test `telnet your_configured_ip port`, `telnet 192.168.1.101 8080`

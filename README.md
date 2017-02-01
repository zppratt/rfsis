# Redundant Failover Seemless IP Stack (RFSIS)
This document loosely describes how to install and operate our demo TCP Echo application for RFSIS.


## One Way Testing Instructions
These instruction describe how to test the TCP Echo application using just one box.

1. `git clone https://github.com/zppratt/rfsis.git`
2. `cd rfsis/lib/picotcp`
3. `make TAP=1`
4. `cd ../../`
5. `make`
6. `sudo ifconfig tap0 192.168.1.100 up`
7. `sudo ./echoserver`

### Note:
The tap device and the IP configuration in config.json need to be different. 

# Redundant Failover Seemless IP Stack (RFSIS)
* Objective: This document loosely describes how to install and operate our demo TCP Echo application for RFSIS, and also how to develop your own applications using RFSIS through our framework.
* Authors: Brice Aldrich, Devin Aspy, Zach Pratt

##Description:
RFSIS is a proof of concept Redundant Failover Seamless IP-Stack. Those are fancy words so for simplification it basically just describes an application used to provide failover services in the event that if a main application server fails, a backup server will effortlessly takeover. Do to time constraints we were unable to create our own IP-stack from scratch, so we used an open source one called PicoTCP and modified it. PicoTCP can be found here: http://www.picotcp.com/.

Our team is building this proof of concept for a Senior CAPSTONE course in the Computer Science program at Purdue University, Fort Wayne. The project is inspired and sponsored by INdigital, a company that specializes in 911 telecommunications.

RFSIS is different from accepted failover solutions today. The idea behind RFSIS is to provide the best possible speeds during the failover process so that the end user will be unaware that any failover ever happened. In order to do this, RFSIS does not failover in the application layer, like most solutions today, but instead fails over in user land. As the application utilizes sockets, the sockets states will be automatically copied by the backup within the IP-Stack.

#### Targeted Audience:
* RFSIS could be used for time sensitive applications such as 911 call routing.
* Internet based security cameras
* Internet based house Alarms
* Anywhere where failover may be needed, use your imagination!

## Instructions for Demo Echo Server
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

## RFSIS Developers Framework
The following will describe how to use our framework as is. It's important to note that because this project is in its infancy, the framework is very limited and will be expanded upon soon if time is in our favor.

#### Configuring RFSIS With config.json

Important Information:
* `config.json` has to be in the conf directory for now. We may change it later to linux standards and put it in /etc
* `config.json` is the main file to set up your preference's on backup and main IP's, devices, and internal stack logic.

`config.json` should look similar to this:

```bash
{
    "backup": false,
    "backup_addr": "192.168.1.12",
    "ipv4_addr": "192.168.1.101",
    "port": 8080,
    "netmask": "255.255.255.0",
    "heartbeat_timer": 3,
    "debug_mode" : true,
    "tap_device_name" : "tap0",
    "main_heartbeats" : false,
}
```
JSON Field Walkthrough:

1. `"backup": boolean` - This describes if the server we are working on is the backup application server or the main. If false, we are configuring the main server.
2. `"backup_addr": "ip_address"` - This describes the backup server's IP address, which will be bound to the backup server's device.
3. `"ipv4_addr": "ip_address"` - This describes the main server's IP address, which will be bound to the main server's device.
4. `"port": integer` - This describes what port the developers application will use.
5. `"netmask": "netmask"` - This describes the netmask to use that will be bound to the device the PicoTCP IP-Stack is using.
6. `"heartbeat_timer": integer` - This describes the interval in which the backup server or the main server should heartbeat in seconds.
7. `"debug_mode": boolean` - This describes wether to run IP-Stack in our custom debug mode, which will print out debug statements to the console as the program is running.
8. `"tap_device_name": "name_of_tap"` - This describes the name of the TAP device picoTCP will use.
9. `"main_heartbeats": boolean` - This describes whether or not the main server will heartbeat the backup, or the backup will heartbeat the main.

#### Developing Applications
For now to develop applications with RFSIS requires knowledge of PicoTCP style sockets. You can find information on http://www.pictotcp.com. In the future I would like to create and add a generic framework where the user defines the style of socket they want in a config, and with the use of callback functions, picoTCP will auto-generate based off the user's options. This will allow the user to develop applications with just basic socket knowledge and no knowledge picoTCP.

Rules:
* Temporarily the picoTCP Stack can only run an application without a return value or parameters. This is an easy fix, but not of high importance right now, because we are still pushing a proof of concept.

Here is an example of how our echo application is injected in the PicoTCP IP-Stack using callbacks:
```cpp
#include "echoserver.hpp"
#include "pico-mod.hpp"

using namespace std;
extern int errno;



int main(void) {
    runPicoStack(start_server);
    return 0;
}
```

Walkthrough:
* We include our application logic if needed (`#include "echosevrer.hpp"`)
* **REQUIRED**: We include or RFSIS library, or pico-mod (`#include "pico-mod.hpp"`)
* In our main of our developed application we run or stack with the callback function, which is the entry point of the app `runPicoStack(start_server)`

How the callback works when starting the picoTCP Stack:

```cpp
using namespace std;
extern int errno;

struct pico_device* init_picotcp();



int runPicoStack(void (*program)()) {
   ..... logic ....

   program();

   .... logic .....

}
```

Walkthrough:
* Take note of the runPicoStack signature. `(void (*program)())`
* Running the stack takes a pointer to a function.
* In the logic of RFSIS that pointer is called, which calls our application

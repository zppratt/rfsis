#!/bin/bash

username=$(whoami)
echo "username is $username"
tdevice=$(sudo tunctl -u $username)
bridgename='br0'
ethname='eth0'
tapname='tap0'

echo "bridge= $bridgename"
echo "ethernet = $ethname"
echo "tapname= $tapname"



sudo brctl addbr $bridgename
echo "A bridge has been added"

sudo brctl addif $bridgename $ethname
echo "eth0 is added to bridge"

sudo brctl addif $bridgename $tapname
echo "tap0 is added to bridge"

sudo ifconfig $bridgename up
echo "up bridge interface"

sudo ifconfig $ethname up
echo "up ethernet interface"

sudo ifconfig $tapname up
echo "up tap interface"

sudo ifconfig $bridgename 192.168.1.75/24
echo "Giving IP address manually to bridge interface"

sudo ifconfig $ethname 0.0.0.0 promisc
echo "setting eth at promisc mode"

sudo ifconfig $tapname 0.0.0.0 promisc
echo "setting tap interface at promisc mode"

sudo ip route add default via 192.168.1.1
echo "setting the gateway for bridge"

iptables -A INPUT -i tap0 -j ACCEPT
iptables -A INPUT -i br0 -j ACCEPT
iptables -A FORWARD -i br0 -j ACCEPT

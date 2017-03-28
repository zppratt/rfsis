#ifndef _HEARTBEAT_HPP_
#define _HEARTBEAT_HPP_

/**
 * Description: This file contains the logic behind an arp request, to see if the other server is alive
 * Author: Brice Aldrich, Devin Aspy, Zach Pratt
 */

#include "std_includes.hpp"

using namespace std;

// Class heartbeat: initalizes the arps to the other servers
class heartbeat{
public:
  heartbeat(string main_ip, struct pico_device *dev, int heartbeat_sec);
  void arp_check();

private:
  string main_ip;
  struct pico_device *dev;
  int heartbeat_sec;
};

/**
 * Constructor: intializes our private fields
 */
heartbeat::heartbeat(string main_ip, struct pico_device *dev, int heartbeat_sec){
  this->main_ip = main_ip; // setting the ip we are going to arp
  this->dev = dev; // setting the device we will use
  this->heartbeat_sec = heartbeat_sec; // the interval in which we will send our arps.
}

/**
 * Sends ARPs to the backup server.
 */
void heartbeat::arp_check(){

    struct pico_ip4 ip;
    pico_string_to_ipv4(main_ip.c_str(), &ip.addr);
    pico_arp_request(dev, &ip, PICO_ARP_QUERY);
}

#endif

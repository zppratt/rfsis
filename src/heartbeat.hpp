#ifndef _HEARTBEAT_HPP_
#define _HEARTBEAT_HPP_

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <thread>

#include "pico_arp.h"
#include "pico_device.h"
#include "pico_ipv4.h"
#include "pico_stack.h"
#include "pico_socket.h"
#include "pico_dev_tap.h"
#include "pico_icmp4.h"

using namespace std;




class heartbeat{
public:
  heartbeat(string main_ip, struct pico_device *dev);
  void startThread();
  static void arp_check();

private:
  static string main_ip;
  struct pico_device *dev;
};

heartbeat::heartbeat(string main_ip, struct pico_device *dev){
  this->main_ip = main_ip;
  this->dev = dev;
}

void heartbeat::startThread(){
  std::thread t_heartbeat (arp_check);
}

void heartbeat::arp_check(){
  struct pico_ip4 ip;
  struct pico_eth *eth = NULL;


  pico_string_to_ipv4(main_ip.c_str(), &ip.addr);

  eth = pico_arp_lookup(&ip);
}


#endif

#ifndef _HEARTBEAT_HPP_
#define _HEARTBEAT_HPP_

#include <stdio.h>
#include <unistd.h>
#include "pico_arp.h"
#include <string>
#include <iostream>
#include <thread>

class heartbeat{
public:
  heartbeat(string main_ip);
  void startThread();
  void arp_check();

private:
  string main_ip;
};

heartbeat::heartbeat(string main_ip){
  this->heartbeat = heartbeat;
}

int heartbeat::startThread(){
  std::thread t_heartbeat (arp_check);
}

void heartbeat::arp_check(){
  struct pico_ip4 ip;
  struct pico_eth *eth = NULL;
  struct pico_arp entry;

  pico_string_to_ipv4(main_ip.c_str(), &ip.addr);
  entry.ipv4 = ip;

  pico_arp_add_entry(&entry);
  entry.arp_status = PICO_ARP_STATUS_STALE;
  eth = pico_arp_lookup(&ip);
}

#endif

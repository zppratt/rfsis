#ifndef _HEARTBEAT_HPP_
#define _HEARTBEAT_HPP_

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <thread>
#include <time.h>

extern "C" {
#include "pico_arp.h"
#include "pico_device.h"
#include "pico_ipv4.h"
#include "pico_stack.h"
#include "pico_socket.h"
#include "pico_dev_tap.h"
#include "pico_icmp4.h"
}

using namespace std;




class heartbeat{
public:
  heartbeat(string main_ip, struct pico_device *dev, int heartbeat_sec);
  void arp_check();
  std::thread arp_checkThread() {
          return std::thread([=] { arp_check(); });
  }

private:
  string main_ip;
  struct pico_device *dev;
  int heartbeat_sec;
};

heartbeat::heartbeat(string main_ip, struct pico_device *dev, int heartbeat_sec){
  this->main_ip = main_ip;
  this->dev = dev;
  this->heartbeat_sec = heartbeat_sec;
}


void heartbeat::arp_check(){

  int count = 1;

  double time_counter = 0;
  clock_t this_time = clock();
  clock_t last_time = this_time;

  while(true){

    this_time = clock();
    time_counter += (double)(this_time - last_time);
    last_time = this_time;

    if(time_counter > (double)(heartbeat_sec * CLOCKS_PER_SEC)) {
          time_counter -= (double)(heartbeat_sec * CLOCKS_PER_SEC);

          struct pico_ip4 ip;
          pico_string_to_ipv4(main_ip.c_str(), &ip.addr);
          pico_arp_request(dev, &ip, PICO_ARP_ANNOUNCE);
          count++;
    }
  }
}



#endif

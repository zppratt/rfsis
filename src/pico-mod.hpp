#ifndef _PICO_MOD_H_
#define _PICO_MOD_H_

#include "std_includes.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

/*
* Description: This file contains the starting point for the IP-Stack. The logic below destinguished between the backup
* server and the main sevrer's stack, and what tasks they are to preform based of the configuration file config.json.
* The stack takes a callback funtion to run a user defined program.
*
* Goal: Proof of Concept Seamless Failover between to application servers.
*
* Authors: Brice Aldrich, Devin Aspy, Zach Pratt.
*/

using namespace std;
extern int errno;

struct pico_device* init_picotcp();



int runPicoStack(void (*program)()) {

    DEBUG_MODE_ON = conf.getDebug_Mode();
    log_debug("[DEBUG:13] echoserver.cpp =======> Debug mode = on"); //Is debug mode on, if it is say something

    if (conf.getBackup()) { //If I am the backup
      log_debug("[DEBUG:16] echoserver.cpp =======> backup = true, server state is backup");

      pico_stack_init(); //Initialize the IP-Stack
      conf.setDev(init_picotcp()); //Create and return our TAP device, set it in our echoHelper for later use.
      //start_server();

      //Listen for Arps
      log_debug("[DEBUG:23] echoserver.cpp =======> main_heartbeats = true, backup will listen for ARPs");


      pico_stack_loop(); //Start our stack loop, read PicoTCP Docs to understand this.

    } else {
      log_debug("[DEBUG:16] echoserver.cpp =======> backup = false, server state is main");
      pico_stack_init(); // Start the picoTcp stack
      log_debug("[DEBUG:36] echoserver.cpp =======> function call = pico_stack_init(), initalizing picoTCP IP-Stack");
      conf.setDev(init_picotcp()); //Create and return our TAP device, set it in our echoHelper for later use.
      program();

      log_debug("[DEBUG:41] echoserver.cpp =======> main_heartbeats = true, main will initalize ARPs");
      heartbeat *hBeat = new heartbeat(conf.getBackup_Addr(), conf.getDev(), conf.getHeartbeat_Timer()); //Start Arping the backup
      std::thread thd1 = hBeat->arp_checkThread();//arp in a thread
      thd1.detach();

        pico_stack_loop();  //Start our stack loop, read PicoTCP Docs to understand this.
    }

    return 0;
}

struct pico_device* init_picotcp(){ //Initializing picoTCP by creating a device and registering it to the stack
  struct pico_device *dev; // Our Pico Device var
  struct pico_ip4 ipaddr, netmask; // Pico uses weird conversions, so these are specific types pico uses for ip-address and netmask

  char * device_name = const_cast<char*>(conf.getTap_Device_name().c_str()); // Let's get our device name from our config parser.
  dev = pico_tap_create(device_name); // Create tap device with the tap name above. Typically tap0.
  log_debug("[DEBUG:79] echoserver.hpp =======> Creating network interface on " + conf.getTap_Device_name());


  if(!dev) { // Check for error
      printf("[ERROR:79] echoserver.hpp =======> Could not create tap device. FATAL!!!!\n");
      return NULL;
  }

  if (conf.getBackup()){ //If we are intializing the stack on the backup server
    pico_string_to_ipv4(conf.getBackup_Addr().c_str(), &ipaddr.addr); // get the backup server address from config parser and convert and copy to pico address
  } else{ //Else we are intializing the stack on the main server
    pico_string_to_ipv4(conf.getIpv4_Addr().c_str(), &ipaddr.addr); // get the main server address from config parser and convert and copy to pico address
  }

  pico_string_to_ipv4(conf.getNetmask().c_str(), &netmask.addr); // Convert the netmask from our config parser to pico netmask
  pico_ipv4_link_add(dev, ipaddr, netmask); // Link'em all together, registering them to the IP-Stack
  log_debug("[DEBUG:90] echoserver.hpp =======> Adding device " + conf.getTap_Device_name() + " PicoTCP's link layer");
  log_debug("[DEBUG:90] echoserver.hpp =======> Adding IP Adress " + conf.getIpv4_Addr() + " PicoTCP's link layer");
  log_debug("[DEBUG:90] echoserver.hpp =======> Adding Netmask " + conf.getNetmask() + " PicoTCP's link layer");

  return dev; //return our device
}

#endif

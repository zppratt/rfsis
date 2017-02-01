#include "std_includes.hpp"
#include "echoserver.hpp"

/*
* Description: This file contains the starting point for the program. The logic below destinguished between the backup
* server and the main, and what tasks they are to preform based of the configuration file config.json.
*
* Goal: Proof of Concept Seamless Failover between to application servers.
*
* Authors: Brice Aldrich, Devin Aspy, Zach Pratt. s
*/

using namespace std;
extern int errno;



int main(void) {

    DEBUG_MODE_ON = conf.getDebug_Mode();
    log_debug("[DEBUG:13] echoserver.cpp =======> Debug mode = on"); //Is debug mode on, if it is say something

    if (conf.getBackup()) { //If I am the backup
      log_debug("[DEBUG:16] echoserver.cpp =======> backup = true, server state is backup");

      pico_stack_init(); //Initialize the IP-Stack
      conf.setDev(init_picotcp()); //Create and return our TAP device, set it in our echoHelper for later use.
      //start_server();

      if(conf.getMain_Heartbeats()){  //If the main is going to ARP for the backup.
        log_debug("[DEBUG:23] echoserver.cpp =======> main_heartbeats = true, backup will listen for ARPs");
      } else{ //If the backup is going to ARP for the main
        log_debug("[DEBUG:23] echoserver.cpp =======> main_heartbeats = false, backup will initialize ARPs");
        heartbeat *hBeat = new heartbeat(conf.getIpv4_Addr(), conf.getDev(), conf.getHeartbeat_Timer()); //Start arping the main
        std::thread thd1 = hBeat->arp_checkThread(); //arp in a thread
      }

      pico_stack_loop(); //Start our stack loop, read PicoTCP Docs to understand this.

    } else {
      log_debug("[DEBUG:16] echoserver.cpp =======> backup = false, server state is main");
      pico_stack_init(); // Start the picoTcp stack
      log_debug("[DEBUG:36] echoserver.cpp =======> function call = pico_stack_init(), initalizing picoTCP IP-Stack");
      conf.setDev(init_picotcp()); //Create and return our TAP device, set it in our echoHelper for later use.
      start_server(); //Start out TCP Echo Server.

      if(conf.getMain_Heartbeats()){ //If the main is going to ARP for the backup.
        log_debug("[DEBUG:41] echoserver.cpp =======> main_heartbeats = true, main will initalize ARPs");
        heartbeat *hBeat = new heartbeat(conf.getBackup_Addr(), conf.getDev(), conf.getHeartbeat_Timer()); //Start Arping the backup
        std::thread thd1 = hBeat->arp_checkThread();//arp in a thread
      } else{ //If the backup is going to ARP for the main
        log_debug("[DEBUG:41] echoserver.cpp =======> main_heartbeats = false, main will listen for ARPs");
      }

        pico_stack_loop();  //Start our stack loop, read PicoTCP Docs to understand this.
    }

    return 0;
}

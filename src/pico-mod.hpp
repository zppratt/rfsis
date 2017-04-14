
#ifndef _PICO_MOD_H_
#define _PICO_MOD_H_

#include "std_includes.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <pico_icmp4.h>

/**
 * Description: This file contains the starting point for the IP-Stack. The logic below distinguished between the backup
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
struct pico_device* spoof_IP();
void app_slaacv4();
void slaacv4_cb(struct pico_ip4 *ip, uint8_t code);
void ping_callback_slaacv4(struct pico_icmp4_stats *s);

void ping_callback_slaacv4(struct pico_icmp4_stats *s)
{
    char host[30] = { };
    printf("Cloning IP test-2.\n");
    pico_ipv4_to_string(host, s->dst.addr);
    if (s->err == 0) {
        dbg("SLAACV4: %lu bytes from %s: icmp_req=%lu ttl=64 time=%lu ms\n", s->size, host,
            s->seq, (long unsigned int)s->time);
        if (s->seq >= 3) {
            dbg("SLAACV4: TEST SUCCESS!\n");
            pico_slaacv4_unregisterip();
            exit(0);
        }
    } else {
        dbg("SLAACV4: ping %lu to %s error %d\n", s->seq, host, s->err);
        dbg("SLAACV4: TEST FAILED!\n");
        exit(1);
    }
}

void slaacv4_cb(struct pico_ip4 *ip, uint8_t code)
{
    printf("Cloning IP test-1.\n");  
    char dst[16] = "169.254.22.5";
    printf("SLAACV4 CALLBACK ip:0x%X code:%d \n", ip->addr, code);
    if (code == 0)
    {
#ifdef PICO_SUPPORT_PING
        pico_icmp4_ping(dst, 3, 1000, 5000, 32, ping_callback_slaacv4);
#else
        exit(0);
#endif
    }
    else
    {
        exit(255);
    }

}


void app_slaacv4()
{
    char *sdev = NULL;
    printf("Cloning IP test.\n");
    struct pico_device *dev = conf.getDev(); // Our Pico Device var

    if(dev == NULL) {
        printf("%s: error getting device %s: %s\n", __FUNCTION__, dev->name, strerror(pico_err));
        exit(255);
    }

    pico_slaacv4_claimip(dev, slaacv4_cb);
}


int runPicoStack(void (*program)()) {
    DEBUG_MODE_ON = conf.getDebug_Mode();
    log_debug("echoserver.cpp =======> Debug mode = on"); //Is debug mode on, if it is say something

    if (conf.getBackup()) { //If I am the backup
        log_debug("echoserver.cpp =======> backup = true, server state is backup");

        pico_stack_init(); //Initialize the IP-Stack
        conf.setDev(init_picotcp()); //Create and return our TAP device, set it in our echoHelper for later use.
      //  program();

        arpSniffer *arpCatch = new arpSniffer();
        std::thread thread = arpCatch->arpSnifferThread();
        thread.detach();

        log_debug("echoserver.cpp =======> main_heartbeats = true, backup will listen for ARPs");

        NetworkMimic mimic;
	int cloneFlag = 0;

        while (1){
          pico_stack_tick();
          sleep(1);
           if (!conf.getBackup() && cloneFlag == 0){
              printf("About to enter clone mac function\n");
               //mimic.clone_mac(conf.getHwaddress().to_string());
               app_slaacv4();

		// spoof_IP();
               conf.setBackup(false);
               program();
               cloneFlag = 1;
	   }
          //printf("Hey I'm working! *********************************\n");
        }


    } else {
        log_debug("echoserver.cpp =======> backup = false, server state is main");
        pico_stack_init(); // Start the picoTcp stack
        log_debug("echoserver.cpp =======> function call = pico_stack_init(), initalizing picoTCP IP-Stack");
        conf.setDev(init_picotcp()); //Create and return our TAP device, set it in our echoHelper for later use.
        program();

        log_debug("echoserver.cpp =======> main_heartbeats = true, main will initalize ARPs");
        Heartbeat *hBeat = new Heartbeat(conf.getBackup_Addr(), conf.getDev(), conf.getHeartbeat_Timer()); //Start Arping the backup
        std::thread thd1 = hBeat->arp_checkThread();//arp in a thread
        thd1.detach();


        pico_stack_loop();  //Start our stack loop, read PicoTCP Docs to understand this.


    }
    return 0;
}

/**
 * Initialize picoTCP by creating a device and registering it to the stack.
 */
struct pico_device* init_picotcp(){

   

    struct pico_device *dev; // Our Pico Device var
    struct pico_ip4 ipaddr, netmask; // Pico uses weird conversions, so these are specific types pico uses for ip-address and netmask

    char * device_name = const_cast<char*>(conf.getTap_Device_name().c_str()); // Let's get our device name from our config parser.
    dev = pico_tap_create(device_name); // Create tap device with the tap name above. Typically tap0.
    log_debug("echoserver.hpp =======> Creating network interface on " + conf.getTap_Device_name());

    if(!dev) { // Check for error
        printf("echoserver.hpp =======> Could not create tap device. FATAL!!!!\n");
        return NULL;
    }

    if (conf.getBackup()){ //If we are intializing the stack on the backup server
        pico_string_to_ipv4(conf.getBackup_Addr().c_str(), &ipaddr.addr); // get the backup server address from config parser and convert and copy to pico address
    if (conf.getBackup()){ //If we are intializing the stack on the backup server
        pico_string_to_ipv4(conf.getBackup_Addr().c_str(), &ipaddr.addr); // get the backup server address from config parser and convert and copy to pico address
    } else{ //Else we are intializing the stack on the main server
        pico_string_to_ipv4(conf.getIpv4_Addr().c_str(), &ipaddr.addr); // get the main server address from config parser and convert and copy to pico address
    }


    pico_string_to_ipv4(conf.getNetmask().c_str(), &netmask.addr); // Convert the netmask from our config parser to pico netmask
    pico_ipv4_link_add(dev, ipaddr, netmask); // Link'em all together, registering them to the IP-Stack

    log_debug("echoserver.hpp =======> Adding device " + conf.getTap_Device_name() + " PicoTCP's link layer");
    log_debug("echoserver.hpp =======> Adding IP Adress " + conf.getIpv4_Addr() + " PicoTCP's link layer");
    log_debug("echoserver.hpp =======> Adding Netmask " + conf.getNetmask() + " PicoTCP's link layer");

    return dev; //return our device
    }
}

struct pico_device* spoof_IP(){
    struct pico_device *dev = conf.getDev(); // Our Pico Device var
    struct pico_ip4 ipaddr, netmask; // Pico uses weird conversions, so these are specific types pico uses for ip-address and netmask

    if(!dev) { // Check for error
        printf("echoserver.hpp =======> Could not spoof ip on tap device. FATAL!!!!\n");
        return NULL;
    }

    pico_string_to_ipv4(conf.getIpv4_Addr().c_str(), &ipaddr.addr); // get the main server address from config parser and convert and copy to pico address
    pico_string_to_ipv4(conf.getNetmask().c_str(), &netmask.addr); // Convert the netmask from our config parser to pico netmask
    pico_ipv4_link_del(dev, ipaddr); // Link'em all together, registering them to the IP-Stack
    pico_ipv4_link_add(dev, ipaddr, netmask);

    log_debug("echoserver.hpp =======> Spoofed Device: " + conf.getTap_Device_name());
    log_debug("echoserver.hpp =======> With IP Address " + conf.getIpv4_Addr());
    log_debug("echoserver.hpp =======> And Netmask " + conf.getNetmask());

    return dev; //return our device
}



#endif

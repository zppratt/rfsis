#ifndef _ARPSNIFFER_HPP_
#define _ARPSNIFFER_HPP_

#include "std_includes.hpp"
#include <tins/tins.h>
#include <map>
#include <iostream>
#include <functional>
#include <chrono>
#include <thread>

using std::cout;
using std::endl;
using std::map;
using std::bind;
using namespace Tins;

class arpSniffer{
public:
  void start();
  std::thread arpSnifferThread() { //Thread this popsicle stand (to maximize profits of course).
          return std::thread([=] { start(); });
  }
  static void threadTimer(int* clonemacFlag, int* arpCount, int timerThreadId);

private:
  bool callback(PDU &some_pdu);
  int macFlag = 0;
  int threadCount = 0;
  bool macCloned = false;
};

void arpSniffer::start(){
    SnifferConfiguration config;
    config.set_promisc_mode(true);
    config.set_filter("arp");

    try {
        // Sniff on the provided interface in promiscuous mode
        Sniffer sniffer(conf.getTap_Device_name().c_str(), config);
        sniffer.sniff_loop(make_sniffer_handler(this, &arpSniffer::callback));
    }
    catch (std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

bool arpSniffer::callback(PDU &some_pdu) {
    const ARP& arp = some_pdu.rfind_pdu<ARP>();

    if (this->macFlag == 1 && this->macCloned == false) {
        NetworkMimic mimic;

        mimic.clone_mac(conf.getHwaddress().to_string());
        this->macCloned = true;
        conf.setDev(mimic.spoof_IP());
    }

    if (arp.sender_ip_addr() == conf.getIpv4_Addr()){
        //Increment threadCount since ARP was received
        this->threadCount++;

        //Create thread timer to check if ARP is received in specified time manner
        std::thread arpTimer(threadTimer, &macFlag, &threadCount, threadCount);
        arpTimer.detach();
        if (arp.target_ip_addr() == conf.getBackup_Addr()){
          cout << "[HEARTBEAT] " << "The senders IP is at: " << arp.sender_ip_addr() << " And the hw address is at: " << arp.sender_hw_addr() << endl;
        }

        conf.setHwaddress(arp.sender_hw_addr());
    }

  // cout << "[TEST] " << "The senders IP is at: " << arp.sender_ip_addr() << " And the hw address is at: " << arp.sender_hw_addr() << endl;

    return true;
}

void arpSniffer::threadTimer(int* clonemacFlag, int* arpCount,
        int timerThreadId) {
    bool sleep = true;

    //printf("\n%s\n", "Thread Created... Sleeping thread");

    auto start = std::chrono::system_clock::now();
    while (sleep) {
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                now - start);

        //1,000,000 microseconds is equal to one second
        if (elapsed.count() > 3500000)
            sleep = false;
    }

    //printf("\nCurrent Thread Count = %d\nTimer Process ID = %d\n", *currentThreadCount, timerThreadId);

    //If equal than no arp from main server in specified.
    if (*arpCount == timerThreadId) {
        //Flips flag so that parent thread knows to clone mac and take over as master
        *clonemacFlag = 1;
        printf("%s\n", "Main Server Time Out");
    }

    return;
}

#endif

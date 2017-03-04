#ifndef _ARPSNIFFER_HPP_
#define _ARPSNIFFER_HPP_


#include "std_includes.hpp"
#include <tins/tins.h>
#include <map>
#include <iostream>
#include <functional>

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
private:
  bool callback(PDU &some_pdu);
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
  if (arp.sender_ip_addr() == conf.getIpv4_Addr()){
    cout << "[TEST] " << "The senders IP is at: " << arp.sender_ip_addr() << " And the hw address is at: " << arp.sender_hw_addr() << endl;
    conf.setHwaddress(arp.sender_hw_addr());
  }

  return true;
}



#endif

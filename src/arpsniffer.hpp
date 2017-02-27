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
  void start(string dev);
  void run(Sniffer& sniffer);
private:
  bool callback(const PDU& pdu);
};

void arpSniffer::start(string dev){
    SnifferConfiguration config;
    config.set_promisc_mode(true);
    config.set_filter("arp");

    try {
        // Sniff on the provided interface in promiscuous mode
        Sniffer sniffer(dev.c_str(), config);
        run(sniffer);
    }
    catch (std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

void arpSniffer::run(Sniffer& sniffer) {
    sniffer.sniff_loop(
        bind(
            &arpSniffer::callback,
            this,
            std::placeholders::_1
        )
    );
}

bool arpSniffer::callback(const PDU& pdu) {

  const ARP& arp = pdu.rfind_pdu<ARP>();
  if (arp.sender_ip_addr() == conf.getIpv4_Addr()){
    cout << "[TEST] " << "The senders IP is at: " << arp.sender_ip_addr() << " And the hw address is at: " << arp.sender_hw_addr() << endl;
    conf.setHwaddress(arp.sender_hw_addr());
  }


}



#endif

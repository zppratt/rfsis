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
            &arp_monitor::callback,
            this,
            std::placeholders::_1
        )
    );
}

bool arpSniffer::callback(const PDU& pdu) {

}



#endif

#ifndef _ARPSNIFFER_HPP_
#define _ARPSNIFFER_HPP_

#include <tins/tins.h>
#include <map>
#include <iostream>
#include <functional>
#include "std_includes.hpp"

using std::cout;
using std::endl;
using std::map;
using std::bind;

using namespace Tins;
using namespace std;

class arp_monitor {
public:
    void run(Sniffer& sniffer);
private:
    bool callback(const PDU& pdu);

    map<IPv4Address, HWAddress<6>> addresses;
};

void arp_monitor::run(Sniffer& sniffer) {
    sniffer.sniff_loop(
        bind(
            &arp_monitor::callback,
            this,
            std::placeholders::_1
        )
    );
}

bool arp_monitor::callback(const PDU& pdu) {
    // Retrieve the ARP layer
    const ARP& arp = pdu.rfind_pdu<ARP>();
    // Is it an ARP reply?
    if (arp.opcode() == ARP::REPLY) {
        // Let's check if there's already an entry for this address
        auto iter = addresses.find(arp.sender_ip_addr());
        if (iter == addresses.end()) {
            // We haven't seen this address. Save it.
            addresses.insert({ arp.sender_ip_addr(), arp.sender_hw_addr()});
            cout << "[INFO] " << arp.sender_ip_addr() << " is at "
                 << arp.sender_hw_addr() << std::endl;
        }
        else {
            // We've seen this address. If it's not the same HW address, inform it
            if (arp.sender_hw_addr() != iter->second) {
                cout << "[WARNING] " << arp.sender_ip_addr() << " is at "
                     << iter->second << " but also at " << arp.sender_hw_addr()
                     << endl;
            }
        }
    }
    return true;
}



#endif

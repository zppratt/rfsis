#ifndef _PACKET_FORWARDER_H
#define _PACKET_FORWARDER_H

#include "std_includes.hpp"
#include <tins/tins.h>
#include <map>
#include <iostream>
#include <functional>

class packetForwarder{
public:
  packetForwarder();
  void start();
  void setFilter(std::string filter);
  void setInterface(std::string interface);
private:
  std::string filter;
  PacketSender sender;
};

packetForwarder::packetForwarder(){
    // Set the default filter.
    filter = "tcp and host" + conf.getIpv4_Addr();
}

/**
 * Configure and start the packet sniffing.
 */
void packetForwarder::start() {
    SnifferConfiguration config;
    config.set_promisc_mode(true); // set to promiscuous mode
    config.set_filter(filter); // set filter
    Sniffer sniffer(conf.getTap_Device_name(), config); // bind them to our sniffer
    sniffer.sniff_loop(make_sniffer_handler(this, &sniff::handle)); // sniff on a loop, call back or handler "handle"
}

/**
 * Sends the IP PDU to the backup if it has the correct protocol and destination.
 */
bool packetForwarder::handle(PDU &some_pdu) { //handle gets the pdu of packet.
    const IP &ip = some_pdu.rfind_pdu<IP>(); // let's get pdu for IP
    if (ip.protocol() == 6){ // is this IP protocl Transport Layer TCP?
      if (ip.dst_addr() == conf.getIpv4_Addr()){
        ip.dst_addr(conf.getBackup_Addr());
        sender.send(ip); // Let's forward the packet containing to the backup
      }
    }
    return true;
}

void packetForwarder::setFilter(std::string filter){
  this->filter = filter;
}
void packetForwarder::setInterface(std::string interface){
  this->interface = interface;
}

#endif

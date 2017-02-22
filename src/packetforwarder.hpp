#ifndef _PACKET_FORWARDER_H
#define _PACKET_FORWARDER_H

#include "std_includes.hpp"


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

packetForwarder::packetForwarder(){ //defualt filter to sniff for
  filter = "tcp and host" + conf.getIpv4_Addr();
}

void packetForwarder::start() { //lets fire up this packet sniffer
    SnifferConfiguration config; //configure
    config.set_promisc_mode(true); // set to promiscuos mode
    config.set_filter(filter); // set filter
    Sniffer sniffer(conf.getTap_Device_name(), config); //bind them to our sniffer
    sniffer.sniff_loop(make_sniffer_handler(this, &sniff::handle)); // sniff on a loop, call back or handler "handle"
}

bool packetForwarder::handle(PDU &some_pdu) { //handle gets the pdu of packet.
    const IP &ip = some_pdu.rfind_pdu<IP>(); // let's get pdu for IP
    if (ip.protocol() == 6){ //is this IP protocl Transport Layer TCP?
      TCP tcp = ip.rfind_pdu<TCP>(); //if it is, get the inner pdu of IP and put it in our TCP object
      const RawPDU& raw = tcp.rfind_pdu<RawPDU>(); // Now let's get the raw data segment, layer 3.
      const RawPDU::payload_type& payload = raw.payload(); // put it in our payload_type helper.
      std::string message( payload.begin(), payload.end() ); // convert the data segment tostring.

      IP pkt = IP(conf.getBackup_Addr() / TCP(22) / RawPDU(message + "KEYABCD1234"); // Let's build our packet with a key to send to the backup. The IP we want to send to, type TCP, and our datasegment + our key.
      sender.send(pkt); //Let's forward the packet containing our key to the backup 


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

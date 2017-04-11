#ifndef SOCKET_SYNC_HPP
#define SOCKET_SYNC_HPP

#include "std_includes.hpp"

class socket_sync_serv{
public:
  socket_sync_serv();
  serv_start();
private:
  struct pico_socket *conn_sock;
  uint16_t port; // our port
  int ret; // For error checking
  struct pico_ip4 dAddr;
 //  struct pico_socket *s;
 // uint8_t loops;
 // uint8_t subloops;
 // uint16_t datasize;
 // uint16_t sport;
 // union pico_address dst;
};

void cb_udpsend(uint16_t ev, struct pico_socket *s) {
   printf("Hello");
  
}

socket_sync_serv::socket_sync_serv() {
   
  pico_string_to_ipv4(conf.getBackup_Addr().c_str(), &dAddr.addr);
  dstPort = short_be(2552);
}

socket_sync_serv::serv_start() {

  conn_sock = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP, &cb_udpsend);

  ret = pico_socket_sendto(sock, "Testing", 7u, ((void *)(&dstAddr)), dstport);
}
#endif

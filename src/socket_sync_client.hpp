#ifndef SOCKET_SYNC_HPP
#define SOCKET_SYNC_HPP

#include "std_includes.hpp"

class socket_sync_client{
public:
  socket_sync_client();
  void start_client();
 // void cb_udpclient(uint16_t ev, struct pico_socket *s);
private:
  struct pico_socket *conn_sock;
  uint16_t port; // our port
  int ret; // For error checking
  struct pico_ip4 address;
  uint16_t listen_port;
  uint8_t dstPort;
  struct pico_ip4 dAddr;
 //  struct pico_socket *s;
 // uint8_t loops;
 // uint8_t subloops;
 // uint16_t datasize;
 // uint16_t sport;
 // union pico_address dst;
};

void cb_udpclient(uint16_t ev, struct pico_socket *s){
   char *recvbuf = NULL;
}

socket_sync_client::socket_sync_client(){
  address = {0};
  listen_port = short_be(0);
  pico_string_to_ipv4(conf.getBackup_Addr().c_str(), &dAddr.addr);
  dstPort = short_be(2552);

}

void socket_sync_client::start_client(){
  conn_sock = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP, &cb_udpclient);

  if (!conn_sock) {
      printf("%s: error opening socket: %s\n", __FUNCTION__, strerror(pico_err));
      exit(1);
  }

  ret = pico_socket_bind(conn_sock, &address, &listen_port);

  if (ret < 0) {
      printf("error binding socket to %s\n", strerror(pico_err));
      exit(1);
  }

  ret = pico_socket_connect(conn_sock, &dAddr, dstPort);

  if (ret < 0) {
      printf("error connecting to %s\n",  strerror(pico_err));
      exit(1);
  }
}

#endif

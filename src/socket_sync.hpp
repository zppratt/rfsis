#ifndef SOCKET_SYNC_HPP
#define SOCKET_SYNC_HPP

#include "std_includes.hpp"

class socket_sync{
public:
  socket_sync();
private:
  struct pico_socket *conn_sock;
  uint16_t port; // our port
  int ret; // For error checking

};

socket_sync::socket_sync(){
  conn_sock = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP,
          &cb_udpclient); //
}

#endif

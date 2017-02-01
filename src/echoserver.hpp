#ifndef _6193_ECHO_SERVER_H_
#define _6193_ECHO_SERVER_H_

#include "std_includes.hpp"
#define BSIZE 2048



struct pico_device* init_picotcp();
void start_server();
void cb_tcpserver(uint16_t ev, struct pico_socket *s);


struct pico_device* init_picotcp(){
  struct pico_device *dev;
  struct pico_ip4 ipaddr, netmask;

  char * device_name = const_cast<char*>(conf.getTap_Device_name().c_str());
  dev = pico_tap_create(device_name);
  log_debug("[DEBUG:237] =======> Creating network interface on " + conf.getTap_Device_name());


  if(!dev) {
      printf("[ERROR:237] =======> Could not create tap device. FATAL!!!!\n");
      return NULL;
  }

  pico_string_to_ipv4(conf.getIpv4_Addr().c_str(), &ipaddr.addr);
  pico_string_to_ipv4(conf.getNetmask().c_str(), &netmask.addr);
  pico_ipv4_link_add(dev, ipaddr, netmask);
  log_debug("[DEBUG:248] =======> Adding device " + conf.getTap_Device_name() + " PicoTCP's link layer");
  log_debug("[DEBUG:248] =======> Adding IP Adress " + conf.getIpv4_Addr() + " PicoTCP's link layer");
  log_debug("[DEBUG:248] =======> Adding Netmask " + conf.getNetmask() + " PicoTCP's link layer");

  return dev;
}

void start_server(){
  struct pico_socket *listen_socket;
  uint16_t port;
  int ret;

  struct pico_ip4 address = {0};
  port = short_be(conf.getPort()); // Have to use their short be thing...

  listen_socket = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP,
          &cb_tcpserver);

  if (!listen_socket) {
      printf("[ERROR:194] =======> Could not open socket. FATAL!!!\n");
      exit(1);
  }

  log_debug("[DEBUG:194] =======> Socket successfully opened");
  log_debug("[DEBUG:204] =======> Socket successfully bound to port " + std::to_string(conf.getPort()));
  ret = pico_socket_bind(listen_socket, &address, &port);
  if (ret < 0) {
      printf("[ERROR:204] =======> Could not bind to socket. FATAL!!!\n");
      exit(1);
  }



  ret = pico_socket_listen(listen_socket, 40);
  if (ret < 0) {
      printf("[ERROR:212] =======> Could not listen to socket. FATAL!!!\n");
      exit(1);
  }
  log_debug("[DEBUG:212} =======> Successfully listening on socket.");
}

void cb_tcpserver(uint16_t ev, struct pico_socket *s){
  char recvbuf[BSIZE];
  int read = 0, written = 0;
  int pos = 0, len = 0;
  struct pico_socket *sock_a;
  struct pico_ip4 orig;
  uint16_t port;
  char peer[30];


  /* process read event, data available */
  if (ev & PICO_SOCK_EV_RD) {
    do {
      read = pico_socket_read(s, recvbuf + len, BSIZE - len);
      if (read > 0)
        len += read;
    } while(read > 0);
  }

  /* process connect event, syn received */
  if (ev & PICO_SOCK_EV_CONN) {
    /* accept new connection request */
    sock_a = pico_socket_accept(s, &orig, &port);
    /* convert peer IP to string */
    pico_ipv4_to_string(peer, orig.addr);
    /* print info */
    printf("Connection established with %s:%d.\n", peer, short_be(port));
  }

  /* process fin event, receiving socket closed */
  if (ev & PICO_SOCK_EV_FIN) {
    printf("Socket closed. Exit normally. \n");
  }

  /* process error event, socket error occured */
  if (ev & PICO_SOCK_EV_ERR) {
    printf("Socket Error received: %s. Bailing out.\n", strerror(pico_err));
    exit(1);
  }

  /* process close event, receiving socket received close from peer */
  if (ev & PICO_SOCK_EV_CLOSE) {
    printf("Socket received close from peer.\n");
    /* shutdown write side of socket */
    pico_socket_shutdown(s, PICO_SHUT_WR);
  }

  /* if data read, echo back */
  if (len > pos) {
    do {
      /* echo data back to peer */
      written = pico_socket_write(s, recvbuf + pos, len - pos);
      if (written > 0) {
        pos += written;
        if (pos >= len) {
          pos = 0;
          len = 0;
          written = 0;
        }
      } else {
        printf("SOCKET> ECHO write failed, dropped %d bytes\n",(len-pos));
      }
    } while(written > 0);
  }
}

#endif

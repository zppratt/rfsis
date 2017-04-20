#ifndef SOCKET_SYNC_HPP
#define SOCKET_SYNC_HPP

#include "std_includes.hpp"

struct udpclient_pas *udpclient_pas;

class socket_sync_client{
public:
  socket_sync_client();
  void start_client();
  std::thread sync_client_thread() { //Thread this popsicle stand (to maximize profits of course).
          return std::thread([=] { start_client(); });
  }

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

void cb_udpclient(uint16_t ev, struct pico_socket *s);

socket_sync_client::socket_sync_client(){
  udpclient_pas = (struct udpclient_pas*)calloc(1, sizeof(struct udpclient_pas));
  address = {0};
  listen_port = short_be(0);
  pico_string_to_ipv4(conf.getBackup_Addr().c_str(), &dAddr.addr);
  dstPort = short_be(2662);

}

void socket_sync_client::start_client(){
  conn_sock = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP, &cb_udpclient);

  if (!conn_sock) {
      printf("%s: error opening socket: %s\n", __FUNCTION__, strerror(pico_err));
      exit(1);
  }

  printf("Socket opened successfully");

  ret = pico_socket_bind(conn_sock, &address, &listen_port);

  if (ret < 0) {
      printf("error binding socket to %s\n", strerror(pico_err));
      exit(1);
  }

  printf("Socket bound successfully");

  ret = pico_socket_connect(conn_sock, &dAddr, dstPort);

  if (ret < 0) {
      printf("error connecting to %s\n",  strerror(pico_err));
      exit(1);
  }

  printf("Socket connected successfully");
}

void cb_udpclient(uint16_t ev, struct pico_socket *s)
{
    char *recvbuf = NULL;
    int r = 0;
    int datasize = 5000;

    if (ev & PICO_SOCK_EV_RD) {
        //recvbuf = calloc(1, udpclient_pas->datasize);
   	recvbuf = (char*)calloc(1, datasize);
    
    if (!recvbuf) {
            printf("%s: no memory available\n", __FUNCTION__);
            return;
        }

        do {
          r = pico_socket_recv(s, recvbuf, datasize);  
	//r = pico_socket_recv(s, recvbuf, udpclient_pas->datasize);
            printf("UDP recieved: %s\n", recvbuf);
	} while ( r > 0);
        free(recvbuf);
    }

    if (ev == PICO_SOCK_EV_ERR) {
        printf("Socket Error received. Bailing out.\n");
        free(udpclient_pas);
        exit(7);
    }
}

#endif

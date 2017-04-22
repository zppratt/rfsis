#ifndef SOCKET_SYNC_HPP
#define SOCKET_SYNC_HPP

#include "std_includes.hpp"

struct udpclient_pas *udpclient_pas;

class socket_sync_client{
public:
 // socket_sync_client();
  void udpecho_start();
  void start_client();
  std::thread sync_client_thread() { //Thread this popsicle stand (to maximize profits of course).
          return std::thread([=] { udpecho_start(); });
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

/*
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
}*/

#include "../lib/picotcp/test/examples/utils.h"
#include <pico_ipv4.h>
#include <pico_ipv6.h>
#include <pico_socket.h>
#include "std_includes.hpp"

/**** START UDP ECHO ****/
/*
 * udpecho expects the following format: udpecho:bind_addr:listen_port[:sendto_port:datasize]
 * bind_addr: IP address to bind to
 * listen_port: port number on which the udpecho listens
 * sendto_port [OPTIONAL]: port number to echo datagrams to (echo to originating IP address)
 * datasize [OPTIONAL]: max size of the data red from the socket in one go
 *
 * REMARK: once an optional parameter is given, all optional parameters need a value!
 *
 * f.e.: ./build/test/picoapp.elf --vde pic0:/tmp/pic0.ctl:10.40.0.3:255.255.255.0: -a udpecho:10.40.0.3:6667:6667:1400
 */

 void cb_udpecho1(uint16_t ev, struct pico_socket *s);
 void deferred_exit1(pico_time __attribute__((unused)) now, void *arg);



static int udpecho_exit1 = 0;

struct udpecho_pas *udpecho_pas1;

/**
 * The call back function for our UDP echo app.
 * @param ev: The protocol as found in "pico_addressing.h".
 * (Examples: PICO_PROTO_IPV4, PICO_PROTO_IPV6, PICO_PROTO_UDP)
 * @param pico_socket: The socket the use to connect.
 */
void cb_udpecho1(uint16_t ev, struct pico_socket *s)
{
    char *recvbuf = NULL;
    uint16_t port = 0;
    int r = 0;
    union {
        struct pico_ip4 ip4;
        struct pico_ip6 ip6;
    } peer;
    if (udpecho_exit1)
        return;

    if (ev & PICO_SOCK_EV_FIN) { // If socket closed
        log_debug("[DEBUG:184] echoserver.hpp =======> Socket closed. Exiting normally!");
        pico_timer_add(2000, deferred_exit1, udpecho_pas1);
    }

    if (ev == PICO_SOCK_EV_RD) {
        recvbuf = (char*)calloc(1, udpecho_pas1->datasize);   //Buffer we will recieve data in
        if (!recvbuf) {
            printf("%s: no memory available\n", __FUNCTION__);
            return;
        }

        do {
            r = pico_socket_recvfrom(s, recvbuf, udpecho_pas1->datasize, (void *)&peer.ip4.addr, &port);
            printf("UDP recvfrom returned %d\n", r);
            if (r > 0) {
                if (strncmp(recvbuf, "end", 3) == 0) {
                    printf("Client requested to exit... test successful.\n");
                    if (!pico_timer_add(1000, deferred_exit1, udpecho_pas1)) {
                        printf("Failed to start exit timer, exiting now\n");
                        exit(1);
                    }
                    udpecho_exit1++;
                }
		
		// char *recvbuf = const_cast<char*>("hello from ACTIVE!".c_str());
               // pico_socket_sendto(s, "Hello from ACTIVE!", 20, (void *)&peer.ip4.addr, port);
            }
        } while (r > 0);
        free(recvbuf);
    }

    if (ev == PICO_SOCK_EV_ERR) {
        printf("Socket Error received. Bailing out.\n");
        free(udpecho_pas1);
        exit(7);
    }

    picoapp_dbg("%s: received packet from %08X:%u\n", __FUNCTION__, long_be(peer), short_be(port));
}

/*
 *Start UDP echoserver
 */
void socket_sync_client::udpecho_start()
{
    
    char *baddr = NULL, *lport = NULL, *sport = NULL, *s_datasize = NULL;
    uint16_t listen_port = short_be(2662);;
    struct pico_ip4 inaddr_bind = {0};

    int ret = 0;

    udpecho_pas1 = (struct udpecho_pas*)calloc(1, sizeof(struct udpecho_pas));
    if (!udpecho_pas1) {
        printf("%s: no memory available\n", __FUNCTION__);
        exit(255);
    }

    udpecho_pas1->s = NULL;
    udpecho_pas1->sendto_port = 0;
    udpecho_pas1->datasize = 5000;

    udpecho_pas1->s = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP, &cb_udpecho1);

    if (!udpecho_pas1->s) {
        printf("%s: error opening socket: %s\n", __FUNCTION__, strerror(pico_err));
        free(udpecho_pas1);
	goto out;
    }

   // ret = pico_socket_bind(udpecho_pas->s, &inaddr_bind, &listen_port);

    //if (ret != 0) {
       // free(udpecho_pas);
       // printf("%s: error binding socket to %08X:%u: %s\n", __FUNCTION__, long_be(inaddr_bind.addr), short_be(listen_port), strerror(pico_err));
	//goto out;
    //}


   // printf("udpecho> Bound to [%s]:%d.\n", inaddr_bind.addr, short_be(listen_port));
 
#ifdef PICOAPP_IPFILTER
    {
        struct pico_ip4 address, in_addr_netmask, in_addr;
        /* struct pico_ipv4_link *link; */
        int ret = 0;
        address.addr = 0x0800280a;
        in_addr_netmask.addr = 0x00FFFFFF;
        in_addr.addr = 0x0000320a;
        /* link = pico_ipv4_link_get(&address); */

        printf("udpecho> IPFILTER ENABLED\n");

        /*Adjust your IPFILTER*/
        ret |= pico_ipv4_filter_add(NULL, 17, NULL, NULL, &in_addr, &in_addr_netmask, 0, 5555, 0, 0, FILTER_DROP);

        if (ret < 0)
            printf("Filter_add invalid argument\n");
    }
#endif

    printf("\n%s: UDP echo launched. Receiving packets of %u bytes on port %u\n\n", __FUNCTION__, udpecho_pas1->datasize, short_be(listen_port));

    /* free strdups */
    if (baddr)
        free (baddr);

    if (lport)
        free (lport);

    if (sport)
        free (sport);

    if (s_datasize)
        free (s_datasize);

    return;

out:
    fprintf(stderr, "udpecho expects the following format: udpecho:bind_addr:listen_port[:sendto_port:datasize]\n");
    free(udpecho_pas1);
    exit(255);
}

void deferred_exit1(pico_time __attribute__((unused)) now, void *arg) { //safe exit
    if (arg) {
        free(arg);
        arg = NULL;
    }

    log_debug("[DEBUG:189] echoserver.hpp =======> Quitting peacefully.");
    exit(0);
}


#endif 


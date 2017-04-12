#ifndef SOCKET_SYNC_HPP
#define SOCKET_SYNC_HPP

#include "std_includes.hpp"

class socket_sync_serv{
public:
  socket_sync_serv();
  serv_start();
private:
  struct pico_socket *conn_sock;
  uint16_t listen_port; // our port
  int ret; // For error checking
  struct pico_ip4 dAddr;
  struct udpclient_pas *udpclient_pas;
 //  struct pico_socket *s;
 // uint8_t loops;
 // uint8_t subloops;
 // uint16_t datasize;
 // uint16_t sport;
 // union pico_address dst;
};
/*
void udpclient_send(pico_time __attribute__((unused)) now, void __attribute__((unused))  *arg);

void cb_udpsend(uint16_t ev, struct pico_socket *s) {
   printf("Hello");
  
}

socket_sync_serv::socket_sync_serv() {
  udpclient_pas = (struct udpclient_pas*)calloc(1, sizeof(struct udpclient_pas));  
  pico_string_to_ipv4(conf.getBackup_Addr().c_str(), &udpclient_pas->dst.ip4.addr);
  udpclient_pas->sport = short_be(2552);
  udpclient_pas->loops = 100;
  udpclient_pas->subloops = 10;
  udpclient_pas->subloops = 1400;
}

socket_sync_serv::serv_start() {
  
   char sinaddr_any[40] = {
        0
    }; 

   udpclient_pas->s = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP, &cb_udpsend);

   pico_ipv4_to_string(sinaddr_any, inaddr_any.addr);

   ret = pico_socket_bind(udpclient_pas->s, &inaddr_any, &udpclient_pas->sport);	

   if (ret < 0) {
      free(udpclient_pas);
      printf("%s: error binding socket to %s:%u: %s\n", __FUNCTION__, sinaddr_any, short_be(listen_port), strerror(pico_err));
      exit(1);
   }

   ret = pico_socket_connect(udpclient_pas->s, &udpclient_pas->dst.ip4, udpclient_pas->sport);

  
   if (ret < 0) {
      printf("%s: error connecting to [%s]:%u: %s\n", __FUNCTION__, daddr, short_be(udpclient_pas->sport), strerror(pico_err));
      free(udpclient_pas);
      exit(1);
   }

   printf("\n%s: UDP client launched. Sending packets of %u bytes in %u loops and %u subloops to %s:%u\n\n",
           __FUNCTION__, udpclient_pas->datasize, udpclient_pas->loops, udpclient_pas->subloops, daddr, short_be(udpclient_pas->sport));

  if (!pico_timer_add(100, udpclient_send, NULL)) {
      printf("Failed to start send timer, sending exit request to echo and exiting\n");
      request_exit_echo((pico_time)0, NULL);
      exit(1);
  }
}

void udpclient_send(pico_time __attribute__((unused)) now, void __attribute__((unused))  *arg)
{
    struct pico_socket *s = udpclient_pas->s;
    char *buf = NULL;
    int i = 0, w = 0;
    static uint16_t loop = 0;

    if (++loop > udpclient_pas->loops) {
        if (!pico_timer_add(1000, request_exit_echo, s)) {
            printf("Failed to start request_exit_echo timer, sending request now\n");
            request_exit_echo((pico_time)0, NULL);
            exit(1);
        }
        return;
    } else {
        buf = calloc(1, udpclient_pas->datasize);
        if (!buf) {
            printf("%s: no memory available\n", __FUNCTION__);
            return;
        }

        memset(buf, '1', udpclient_pas->datasize);
        picoapp_dbg("%s: performing loop %u\n", __FUNCTION__, loop);
        for (i = 0; i < udpclient_pas->subloops; i++) {
            w =  pico_socket_send(s, buf, udpclient_pas->datasize);
            if (w <= 0)
                break;
        }
        picoapp_dbg("%s: written %u byte(s) in each of %u subloops\n", __FUNCTION__, udpclient_pas->datasize, i);
        free(buf);
    }

    if (!pico_timer_add(100, udpclient_send, NULL)) {
        printf("Failed to start send timer, sending exit request to echo and exiting\n");
        request_exit_echo((pico_time)0, NULL);
        exit(1);
    }
}
*/
#endif

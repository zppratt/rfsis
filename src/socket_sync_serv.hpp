#ifndef SOCKET_SYNC_SERV_HPP
#define SOCKET_SYNC_SERV_HPP

#include "std_includes.hpp"

class socket_sync_serv{
public:
  socket_sync_serv();
  void serv_start();
  std::thread sync_serv_thread() { //Thread this popsicle stand (to maximize profits of course).
      return std::thread([=] { serv_start(); });
  }

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

void udpclient_send(pico_time __attribute__((unused)) now, void __attribute__((unused))  *arg);
static void request_exit_echo(pico_time now, void *arg);

static int exit_retry = 0;
struct pico_ip4 inaddr_any;

void cb_udpsend(uint16_t ev, struct pico_socket *s) {
   printf("Hello");
  
}

socket_sync_serv::socket_sync_serv() {
  udpclient_pas = (struct udpclient_pas*)calloc(1, sizeof(struct udpclient_pas));  
  pico_string_to_ipv4(conf.getBackup_Addr().c_str(), &udpclient_pas->dst.ip4.addr);
  inaddr_any = {
    0
  };
  udpclient_pas->sport = short_be(2552);
  udpclient_pas->loops = 100;
  udpclient_pas->subloops = 10;
  //udpclient_pas->subloops = 1400;
}

void dummy_cb(uint16_t __attribute__((unused)) ev, struct pico_socket __attribute__((unused)) *s ) {}

void socket_sync_serv::serv_start() {
  
   struct pico_ip4 inaddr_dst = {};
   uint16_t dport;
   struct pico_socket *sock;
   int ret;
   
   sock = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP, &dummy_cb);

   pico_string_to_ipv4(conf.getBackup_Addr().c_str(), &inaddr_dst.addr);
   dport = short_be(2662);

   ret = pico_socket_sendto(sock, "Testing", 7u, (void *)&inaddr_dst, dport);
   if (ret < 0) printf("Failure in first pico_socket_send\n");
}

void udpclient_send(pico_time __attribute__((unused)) now, void __attribute__((unused))  *arg)
{
    struct pico_socket *s = udpclient_pas->s;
    char *buf;
    int i = 0, w = 0;
    static uint16_t loop = 0;

    printf("Sending UDP message");

    if (++loop > udpclient_pas->loops) {
        if (!pico_timer_add(1000, request_exit_echo, s)) {
            printf("Failed to start request_exit_echo timer, sending request now\n");
            request_exit_echo((pico_time)0, NULL);
            exit(1);
        }
        return;
    } else {
        buf = (char*)calloc(1, udpclient_pas->datasize);
        if (!buf) {
            printf("%s: no memory available\n", __FUNCTION__);
            return;
        }

	udpclient_pas->datasize = 10;
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

    printf("UDP message sent");
}


static void request_exit_echo(pico_time now, void *arg)
{
    struct pico_socket *s = (struct pico_socket *)arg;
    char end[4] = "end";
    pico_socket_send(s, end, 4);
    if (exit_retry++ > 3) {
        if (!pico_timer_add(1000, deferred_exit, udpclient_pas)) {
            printf("Failed to start exit timer, exiting now\n");
            exit(1);
        }
    } else {
        if (!pico_timer_add(1000, request_exit_echo, s)) {
            printf("Failed to start request_exit_echo timer, sending request now\n");
            request_exit_echo((pico_time)0, NULL);
            exit(1);
        }
        printf("%s: requested exit of echo\n", __FUNCTION__);
    }
}

#endif

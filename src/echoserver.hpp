#ifndef _6193_ECHO_SERVER_H_
#define _6193_ECHO_SERVER_H_

#include "std_includes.hpp"
#define BSIZE 2048
struct pico_device* init_picotcp();
void start_server();
void cb_tcpserver(uint16_t ev, struct pico_socket *s);
int send_resp(struct pico_socket *s);
void deferred_exit(pico_time __attribute__((unused)) now, void *arg);

class echoHelper{
public:
  echoHelper();
  void setRead(int read);
  void setPos(int pos);
  void setLen(int len);
  void setFlag(int flag);
  int getRead();
  int getPos();
  int getLen();
  int getFlag();
private:
  int read;
  int pos;
  int len;
  int flag;
};

echoHelper::echoHelper(){
  read = 0;
  pos = 0;
  len = 0;
  flag = 0;
}

void echoHelper::setRead(int read){
  this->read = read;
}

void echoHelper::setPos(int pos){
  this->pos = pos;
}

void echoHelper::setLen(int len){
  this->len = len;
}

void echoHelper::setFlag(int flag){
  this->flag = flag;
}

int echoHelper::getRead(){
  return read;
}

int echoHelper::getPos(){
  return pos;
}

int echoHelper::getLen(){
  return len;
}

int echoHelper::getFlag(){
  return flag;
}

echoHelper help;
char recvbuf[BSIZE];



struct pico_device* init_picotcp(){
  struct pico_device *dev;
  struct pico_ip4 ipaddr, netmask;

  char * device_name = const_cast<char*>(conf.getTap_Device_name().c_str());
  dev = pico_tap_create(device_name);
  log_debug("[DEBUG:79] echoserver.hpp =======> Creating network interface on " + conf.getTap_Device_name());


  if(!dev) {
      printf("[ERROR:79] echoserver.hpp =======> Could not create tap device. FATAL!!!!\n");
      return NULL;
  }

  pico_string_to_ipv4(conf.getIpv4_Addr().c_str(), &ipaddr.addr);
  pico_string_to_ipv4(conf.getNetmask().c_str(), &netmask.addr);
  pico_ipv4_link_add(dev, ipaddr, netmask);
  log_debug("[DEBUG:90] echoserver.hpp =======> Adding device " + conf.getTap_Device_name() + " PicoTCP's link layer");
  log_debug("[DEBUG:90] echoserver.hpp =======> Adding IP Adress " + conf.getIpv4_Addr() + " PicoTCP's link layer");
  log_debug("[DEBUG:90] echoserver.hpp =======> Adding Netmask " + conf.getNetmask() + " PicoTCP's link layer");

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
      printf("[ERROR:106] echoserver.hpp =======> Could not open socket. FATAL!!!\n");
      exit(1);
  }

  log_debug("[DEBUG:106] echoserver.hpp =======> Socket successfully opened");

  ret = pico_socket_bind(listen_socket, &address, &port);
  if (ret < 0) {
      printf("[ERROR:116] echoserver.hpp =======> Could not bind to socket. FATAL!!!\n");
      exit(1);
  }
  log_debug("[DEBUG:116] echoserver.hpp =======> Socket successfully bound to port " + std::to_string(conf.getPort()) + " ret = " + std::to_string(ret));



  ret = pico_socket_listen(listen_socket, 40);
  if (ret < 0) {
      printf("[ERROR:125] echoserver.hpp =======> Could not listen to socket. FATAL!!!\n");
      exit(1);
  }
  log_debug("[DEBUG:125] echoserver.hpp =======> Successfully listening on socket. ret = " + std::to_string(ret));
}

void cb_tcpserver(uint16_t ev, struct pico_socket *s) {

    log_debug("[DEBUG:133] echoserver.hpp =======> I heard something, I better send it back!");

    if (ev & PICO_SOCK_EV_RD) {
        if (help.getFlag() & PICO_SOCK_EV_CLOSE) {
            log_debug("[DEBUG:138] echoserver.hpp =======> fin received!");
        }

        while (help.getLen() < BSIZE) {
            help.setRead(pico_socket_read(s, recvbuf + help.getLen(), BSIZE - help.getLen()));
            if (help.getRead() > 0) {
                help.setLen(help.getLen() + help.getRead());
                help.setFlag(help.getFlag() & ~(PICO_SOCK_EV_RD));
            } else {
                help.setFlag(help.getFlag() | PICO_SOCK_EV_RD);
                break;
            }
        }

        if (help.getFlag() & PICO_SOCK_EV_WR) {
            help.setFlag(help.getFlag() & ~PICO_SOCK_EV_WR);
            send_resp(s);
        }
    }

    if (ev & PICO_SOCK_EV_CONN) {
        struct pico_socket *sock_a = {0};
        struct pico_ip4 orig       = {0};
        uint16_t port              = 0;
        char peer[30]              = {0};
        uint32_t ka_val            = 0;
        int yes                    = 1;

        sock_a = pico_socket_accept(s, &orig, &port);

        pico_ipv4_to_string(peer, orig.addr);
        printf("Connection established with %s:%d\n", peer, short_be(port));
        pico_socket_setoption(sock_a, PICO_TCP_NODELAY, &yes);

        ka_val = 5;
        pico_socket_setoption(sock_a, PICO_SOCKET_OPT_KEEPCNT, &ka_val);

        ka_val = 30000;
        pico_socket_setoption(sock_a, PICO_SOCKET_OPT_KEEPIDLE, &ka_val);

        ka_val = 5000;
        pico_socket_setoption(sock_a, PICO_SOCKET_OPT_KEEPINTVL, &ka_val);

    }

    if (ev & PICO_SOCK_EV_FIN) {
        log_debug("[DEBUG:184] echoserver.hpp =======> Socket closed. Exiting normally!");
        pico_timer_add(2000, deferred_exit, NULL);
    }

    if (ev & PICO_SOCK_EV_ERR) {
        log_debug("[DEBUG:189] echoserver.hpp =======> Socket error, better quit while I'm agead!");
        exit(1);
    }

    if (ev & PICO_SOCK_EV_CLOSE) {
        log_debug("[DEBUG:189] echoserver.hpp =======> Socket received close from peer");
        if (help.getFlag() & PICO_SOCK_EV_RD) {
            pico_socket_shutdown(s, PICO_SHUT_WR);
            log_debug("[DEBUG:189] echoserver.hpp =======> Called shutdown.");
        }

    }

    if (ev & PICO_SOCK_EV_WR) {
        help.setRead(send_resp(s));
        if (help.getRead() == 0) {
            help.setFlag(help.getFlag() | PICO_SOCK_EV_WR);
        } else {
            help.setFlag(help.getFlag() & (~PICO_SOCK_EV_WR));
        }

    }
}

int send_resp(struct pico_socket *s) {
    int w, ww = 0;

    if (help.getLen() >  help.getPos()) {
        do {
            w = pico_socket_write(s, recvbuf + help.getPos(), help.getLen() - help.getPos());
            if (w > 0) {
                help.setPos(help.getPos() + w);
                ww += w;
                if (help.getPos() >= help.getLen()) {
                    help.setPos(0);
                    help.setLen(0);
                }
            }
        } while ((w > 0) && (help.getPos() < help.getLen()));
    }

    return ww;
}

void deferred_exit(pico_time __attribute__((unused)) now, void *arg) {
    if (arg) {
        free(arg);
        arg = NULL;
    }

    log_debug("[DEBUG:189] echoserver.hpp =======> Quitting peacefully.");
    exit(0);
}

#endif

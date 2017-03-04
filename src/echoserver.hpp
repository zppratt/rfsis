#ifndef _6193_ECHO_SERVER_H_
#define _6193_ECHO_SERVER_H_

/*
* Description: This file contains the bulk of the code for the tcp echo application. The file also contains the prep code
* to run the application in the picoTCP IP-Stack.
*
* TODO: Clean this file up to be more OOP.
*
* Authors: Brice Aldrich, Devin Aspy, Zach Pratt
*/

#include "std_includes.hpp" //Standard includes that we use a lot, some gloabl vars, and one gloabl debug function
#define BSIZE 2048 //buffer size for TCP Echo

//Function Declarations
void start_server();
void cb_tcpserver(uint16_t ev, struct pico_socket *s);
int send_resp(struct pico_socket *s);
void deferred_exit(pico_time __attribute__((unused)) now, void *arg);


/*
* Class: echoHelper
* Description: This class is used to set and get multiple variables
* used in our TCP echo application throughout multiple functions.
*/

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

echoHelper::echoHelper(){ //Constructor for echoHelper class
  read = 0; //set all our private ints to 0
  pos = 0;
  len = 0;
  flag = 0;
}

void echoHelper::setRead(int read){ // read setter
  this->read = read;
}

void echoHelper::setPos(int pos){ // pos setter
  this->pos = pos;
}

void echoHelper::setLen(int len){ // len setter
  this->len = len;
}

void echoHelper::setFlag(int flag){ // flag setter
  this->flag = flag;
}

int echoHelper::getRead(){ // read getter
  return read;
}

int echoHelper::getPos(){ // pos getter
  return pos;
}

int echoHelper::getLen(){ // len getter
  return len;
}

int echoHelper::getFlag(){ // flag getter
  return flag;
}

echoHelper help; // Let's creater an echoHelper object named help
char recvbuf[BSIZE]; // Declare our buffer of the size above for use in the TCP Echo app


void start_server(){ // The bulk of the code for the TCP Echo app
  struct pico_socket *listen_socket; // Creating a pico listening socket
  uint16_t port; // our port
  int ret; // For error checking

  struct pico_ip4 address = {0}; // our listening adress
  port = short_be(conf.getPort()); // Have to use their short be thing...

  listen_socket = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP,
          &cb_tcpserver); //Open the listening socket and call our call back function cb_tcpserver

  if (!listen_socket) { // Oops we failed to open our socket ):
      printf("[ERROR:106] echoserver.hpp =======> Could not open socket. FATAL!!!\n");
      exit(1);
  }

  log_debug("[DEBUG:106] echoserver.hpp =======> Socket successfully opened");

  ret = pico_socket_bind(listen_socket, &address, &port); // Bind the listening socket to the address and port connected.
  if (ret < 0) {
      printf("[ERROR:116] echoserver.hpp =======> Could not bind to socket. FATAL!!!\n");
      exit(1);
  }
  log_debug("[DEBUG:116] echoserver.hpp =======> Socket successfully bound to port " + std::to_string(conf.getPort()) + " ret = " + std::to_string(ret));



  ret = pico_socket_listen(listen_socket, 40); // Lets start listening.
  if (ret < 0) { // We couldn't listen, like many of our beloved polititions. ):
      printf("[ERROR:125] echoserver.hpp =======> Could not listen to socket. FATAL!!!\n");
      exit(1);
  }
  log_debug("[DEBUG:125] echoserver.hpp =======> Successfully listening on socket. ret = " + std::to_string(ret));
}

void cb_tcpserver(uint16_t ev, struct pico_socket *s) { //The call back function for our TCP echo app

    log_debug("[DEBUG:133] echoserver.hpp =======> I heard something, I better send it back!");

    if (ev & PICO_SOCK_EV_RD) { // Is there data available
        if (help.getFlag() & PICO_SOCK_EV_CLOSE) {
            log_debug("[DEBUG:138] echoserver.hpp =======> fin received!");
        }

        while (help.getLen() < BSIZE) { //Let's read the data
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
            if (conf.getActive())
              send_resp(s); // Send our response to the data.
        }
    }

    if (ev & PICO_SOCK_EV_CONN) {
        struct pico_socket *sock_a = {0};
        struct pico_ip4 orig       = {0};
        uint16_t port              = 0;
        char peer[30]              = {0};
        uint32_t ka_val            = 0;
        int yes                    = 1;

        sock_a = pico_socket_accept(s, &orig, &port); //Accept a connection

        pico_ipv4_to_string(peer, orig.addr); // get connection origin info
        printf("Connection established with %s:%d\n", peer, short_be(port));
        pico_socket_setoption(sock_a, PICO_TCP_NODELAY, &yes);

        ka_val = 5;
        pico_socket_setoption(sock_a, PICO_SOCKET_OPT_KEEPCNT, &ka_val);

        ka_val = 30000;
        pico_socket_setoption(sock_a, PICO_SOCKET_OPT_KEEPIDLE, &ka_val);

        ka_val = 5000;
        pico_socket_setoption(sock_a, PICO_SOCKET_OPT_KEEPINTVL, &ka_val);

    }

    if (ev & PICO_SOCK_EV_FIN) { // If socket closed
        log_debug("[DEBUG:184] echoserver.hpp =======> Socket closed. Exiting normally!");
        pico_timer_add(2000, deferred_exit, NULL);
    }

    if (ev & PICO_SOCK_EV_ERR) { //If socket error
        log_debug("[DEBUG:189] echoserver.hpp =======> Socket error, better quit while I'm agead!");
        exit(1);
    }

    if (ev & PICO_SOCK_EV_CLOSE) { // If scoket closed from peer
        log_debug("[DEBUG:189] echoserver.hpp =======> Socket received close from peer");
        if (help.getFlag() & PICO_SOCK_EV_RD) {
            pico_socket_shutdown(s, PICO_SHUT_WR);
            log_debug("[DEBUG:189] echoserver.hpp =======> Called shutdown.");
        }

    }

    if (ev & PICO_SOCK_EV_WR) {
        if (conf.getActive())
          help.setRead(send_resp(s));
        if (help.getRead() == 0) {
            help.setFlag(help.getFlag() | PICO_SOCK_EV_WR);
        } else {
            help.setFlag(help.getFlag() & (~PICO_SOCK_EV_WR));
        }

    }
}

int send_resp(struct pico_socket *s) { //send the response
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

void deferred_exit(pico_time __attribute__((unused)) now, void *arg) { //safe exit
    if (arg) {
        free(arg);
        arg = NULL;
    }

    log_debug("[DEBUG:189] echoserver.hpp =======> Quitting peacefully.");
    exit(0);
}

#endif

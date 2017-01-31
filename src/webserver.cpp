#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <fstream>
#include <errno.h>
#include <iostream>
#include <string>

extern "C" {
#include "pico_device.h"
#include "pico_ipv4.h"
#include "pico_stack.h"
#include "pico_socket.h"
#include "pico_dev_tap.h"
#include "pico_icmp4.h"
}

#include "webserver.h"
#include "heartbeat.hpp"
#include "ConfigParser.hpp"

using namespace std;

string p_website = "src/website.html";

/**
 * Whether debug mode is enabled.
 */

bool DEBUG_MODE_ON = false;


extern int errno;

struct web_server serv = {
    NULL,
    0,
    0,
    0,
    0,
    8080,
    "192.168.1.101",
    "255.255.255.0"
};

void log_debug(string message) {
  if (DEBUG_MODE_ON) {
    cout << message << endl;
  }
}

void deferred_exit(pico_time __attribute__((unused)) now, void *arg) {
    if (arg) {
        free(arg);
        arg = NULL;
    }

    printf("%s: quitting\n", __FUNCTION__);
    exit(0);
}

int read_html_file(struct pico_socket *s) {
    // Since the compiler cant tell the difference between read and read
    int red = 0;
    char buffer[BSIZE];

    // We want to use a file descriptors because we want to treat
    // the file as a stream instead of with handlers
    int fd = open(p_website.c_str(), O_RDONLY);
    if(fd < 0) {
        printf("ERROR: %s\n", strerror(errno));
        return -1;
    }

    while((red = read(fd, buffer, sizeof(buffer))) != 0) {
        if(red < 0) {
            printf("ERROR: %s\n", strerror(errno));
            return -1;
        }
        pico_socket_write(s, buffer, red);
    }

    close(fd);

    return red;
}

void cb_tcpserver(uint16_t ev, struct pico_socket *s) {

    printf("tcpecho | env = %u\n", ev);

    if (ev & PICO_SOCK_EV_RD) {
        if (serv.flag & PICO_SOCK_EV_CLOSE) {
            printf("Fin received!\n");
        }

        while (serv.len < BSIZE) {
            serv.read = pico_socket_read(s, serv.recvbuf + serv.len, BSIZE - serv.len);
            if (serv.read > 0) {
                serv.len += serv.read;
                serv.flag &= ~(PICO_SOCK_EV_RD);
            } else {
                serv.flag |= PICO_SOCK_EV_RD;
                break;
            }
        }

        if (serv.flag & PICO_SOCK_EV_WR) {
            serv.flag &= ~PICO_SOCK_EV_WR;
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
        printf("Socket closed. Exiting normally\n");
        pico_timer_add(2000, deferred_exit, NULL);
    }

    if (ev & PICO_SOCK_EV_ERR) {
        printf("Socket error: %s. Time to quit this shiznit!\n",
                strerror(pico_err));
        exit(1);
    }

    if (ev & PICO_SOCK_EV_CLOSE) {
        printf("Socket received close from peer.\n");
        if (serv.flag & PICO_SOCK_EV_RD) {
            pico_socket_shutdown(s, PICO_SHUT_WR);
            printf("Called shutdown write, ev = %d\n", ev);
        }

    }

    if (ev & PICO_SOCK_EV_WR) {
        read_html_file(s);
        pico_socket_shutdown(s, PICO_SHUT_WR);
    }
}


int send_resp(struct pico_socket *s) {
    int w, ww = 0;

    if (serv.len >  serv.pos) {
        do {
            w = pico_socket_write(s, serv.recvbuf + serv.pos, serv.len - serv.pos);
            if (w > 0) {
                serv.pos += w;
                ww += w;
                if (serv.pos >= serv.len) {
                    serv.pos = 0;
                    serv.len = 0;
                }
            }
        } while ((w > 0) && (serv.pos < serv.len));
    }

    return ww;
}

void setup_server(void) {
    struct pico_socket *listen_socket;
    uint16_t port;
    int ret;

    struct pico_ip4 address = {0};
    port = short_be(serv.port); // Have to use their short be thing...

    listen_socket = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP,
            &cb_tcpserver);

    if (!listen_socket) {
        printf("Could not open socket!\n");
        exit(1);
    }
    log_debug("Socket successfully opened.");

    ret = pico_socket_bind(listen_socket, &address, &port);
    if (ret < 0) {
        printf("could not bind socket\n");
        exit(1);
    }
    std::string s;
    s.append("Socket successfully bound to port ");
    s.append(std::to_string(port));
    log_debug(s);

    ret = pico_socket_listen(listen_socket, 40);
    if (ret < 0) {
        printf("could not listen\n");
        exit(1);
    }
    log_debug("Successfully listening on socket.");
}

struct pico_device* init_picotcp(void) {
    struct pico_device *dev;
    struct pico_ip4 ipaddr, netmask;

    char * device_name = conf.getTap_Device_name().c_str();
    dev = pico_tap_create(device_name);
    log_debug("[DEBUG:237] =======> Creating network interface on " + conf.getTap_Device_name());

    free(device_name);
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

int main(void) {

    DEBUG_MODE_ON = conf.getDebug_Mode();
    log_debug("[DEBUG:287] =======> Debug mode = on"); //Is debug mode on, if it is say something

    if (conf.getBackup()) {
      log_debug("[DEBUG:296] =======> backup = true, server state is backup");
      if(conf.getMain_Heartbeats()){
        log_debug("[DEBUG:299] =======> main_heartbeats = true, backup will listen for ARPs");
      } else{
        log_debug("[DEBUG:299] =======> main_heartbeats = false, backup will initialize ARPs");
      }
    } else {
      log_debug("[DEBUG:304] =======> backup = false, server state is main");
      // Start the picoTcp stack
      pico_stack_init();
      log_debug("[DEBUG:307] =======> function call = pico_stack_init(), initalizing picoTCP IP-Stack");
      serv.dev = init_picotcp();
      setup_server();

      if(conf.getMain_Heartbeats()){
        log_debug("[DEBUG:299] =======> main_heartbeats = true, main will initalize ARPs");
        heartbeat *hBeat = new heartbeat(conf.getBackup_Addr(), serv.dev, conf.getHeartbeat_Timer());
        std::thread thd1 = hBeat->arp_checkThread();
      } else{
        log_debug("[DEBUG:299] =======> main_heartbeats = false, main will listen for ARPs");
      }

        pico_stack_loop();
    }

    return 0;
}

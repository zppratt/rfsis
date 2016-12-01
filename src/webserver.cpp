#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <fstream>
#include <errno.h>
#include <iostream>
#include <string>

#include "pico_device.h"
#include "pico_ipv4.h"
#include "pico_stack.h"
#include "pico_socket.h"
#include "pico_dev_tap.h"
#include "pico_icmp4.h"

#include "webserver.h"
#include "heartbeat.hpp"
#include "json.hpp"

// for convenience
using json = nlohmann::json;
using namespace std;

char *p_website = "src/website.html";

extern int errno;

json config;

struct web_server serv = {
    dev       : NULL,
    read      : 0,
    pos       : 0,
    len       : 0,
    flag      : 0,
    port      : 8080,
    ipv4_addr : "192.168.1.101",
    netmask   : "255.255.255.0"
};

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
    int fd = open(p_website, O_RDONLY);
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

    ret = pico_socket_bind(listen_socket, &address, &port);
    if (ret < 0) {
        printf("could not bind socket\n");
        exit(1);
    }

    ret = pico_socket_listen(listen_socket, 40);
    if (ret < 0) {
        printf("could not listen\n");
        exit(1);
    }
}

struct pico_device* init_picotcp(void) {
    struct pico_device *dev;
    struct pico_ip4 ipaddr, netmask, local;

    pico_string_to_ipv4("wlan0", &local.addr);

    dev = pico_ipv4_link_find(&local);

    if(!dev) {
        printf("FAIL!\n");
        return NULL;
    }

    pico_string_to_ipv4(serv.ipv4_addr, &ipaddr.addr);
    pico_string_to_ipv4(serv.netmask, &netmask.addr);
    pico_ipv4_link_add(dev, ipaddr, netmask);

    return dev;
}

void read_config() {
    ifstream myfile ("config.json");
    if (myfile.is_open())
    {
        string content( (std::istreambuf_iterator<char>(myfile) ),
                (std::istreambuf_iterator<char>()    ) );
        config = json::parse(content);
        myfile.close();
    }
    else cout << "Unable to open file";
}

int main(void) {

    heartbeat hBeat("192.168.1.12");
    read_config();

    // Am I the master or the slave?
    auto is_backup = config["backup"];

    if (is_backup) {
        hBeat.startThread();
    }
    else {
        // TODO do masterly deeds
    }

    pico_stack_init();

    serv.dev = init_picotcp();
    if(!serv.dev) {
        printf("Could not make device!\n");
        return -1;
    }

    setup_server();

    pico_stack_loop();

    return 0;
}

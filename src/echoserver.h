#ifndef _6193_ECHO_SERVER_H_
#define _6193_ECHO_SERVER_H_

#define BSIZE 2048

// Might be able to use this later when we start to actually do stuff
struct echo_server {
    struct pico_device *dev;
    int read;
    int pos;
    int len;
    int flag;
    char recvbuf[BSIZE];
} web_server;



struct pico_device* init_picotcp();

void setup_server();
int send_resp(struct pico_socket *s);
void cb_tcpserver(uint16_t ev, struct pico_socket *s);

#endif

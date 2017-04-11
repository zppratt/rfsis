#ifndef _STD_INCLUDES_H_
#define _STD_INCLUDES_H_

#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <fstream>
#include <errno.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>



extern "C" {
#include "pico_arp.h"
#include "pico_device.h"
#include "pico_ipv4.h"
#include "pico_stack.h"
#include "pico_socket.h"
#include "pico_dev_tap.h"
#include "pico_icmp4.h"
#include <sys/ioctl.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
}

#include "ConfigParser.hpp"
ConfigParser conf;
 bool DEBUG_MODE_ON;
void log_debug(std::string message) {
    if (DEBUG_MODE_ON) {
        cout << message << endl;
    }
}

#include "heartbeat.hpp"
#include "networkmimic.hpp"
#include "arpsniffer.hpp"
#include "socket_sync_client.hpp"
 // #include "packetforwarder.hpp"

/**
 * Whether debug mode is enabled.
 */


// #include "arpsniffer.hpp"

extern int mainServerTimeoutFlag;

#endif

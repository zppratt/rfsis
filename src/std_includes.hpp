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
}

#include "heartbeat.hpp"
#include "ConfigParser.hpp"
#include "clonemac.hpp"


ConfigParser conf;

#include "arpsniffer.hpp"


/**
 * Whether debug mode is enabled.
 */

bool DEBUG_MODE_ON;


// #include "arpsniffer.hpp"

void log_debug(string message) {
  if (DEBUG_MODE_ON) {
    cout << message << endl;
  }
}

#endif

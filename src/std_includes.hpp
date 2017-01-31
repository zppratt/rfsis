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

/**
 * Whether debug mode is enabled.
 */

extern bool DEBUG_MODE_ON;
extern ConfigParser conf;

void log_debug(string message) {
  if (DEBUG_MODE_ON) {
    cout << message << endl;
  }
}

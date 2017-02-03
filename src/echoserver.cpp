#include "std_includes.hpp"
#include "echoserver.hpp"
#include "pico-mod.hpp"

/*
* Description: This file contains the starting point for the program. The logic below destinguished between the backup
* server and the main, and what tasks they are to preform based of the configuration file config.json.
*
* Goal: Proof of Concept Seamless Failover between to application servers.
*
* Authors: Brice Aldrich, Devin Aspy, Zach Pratt. s
*/

using namespace std;
extern int errno;



int main(void) {
    runPicoStack(start_server);
    return 0;
}

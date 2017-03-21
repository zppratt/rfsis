#include "echohelper.hpp"
#include "std_includes.hpp"
#include "rfsis.hpp"

/**
 * Description: This is our testing application. We start our IP-Stack and pass our logic to it. Eureka!
 *
 * Goal: TCP Echo using PicoTCP IP-Stack.
 *
 * TODO: Framework documentation
 *
 * Authors: Brice Aldrich, Devin Aspy, Zach Pratt.
 */

using namespace std;
extern int errno;

int main(void) {
    runStack(start_server);
    return 0;
}

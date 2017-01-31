#include "std_includes.hpp"

using namespace std;
extern int errno;

bool DEBUG_MODE_ON;
ConfigParser conf;

int main(void) {

    DEBUG_MODE_ON = conf.getDebug_Mode();
    log_debug("[DEBUG:287] =======> Debug mode = on"); //Is debug mode on, if it is say something

    if (conf.getBackup()) {
      log_debug("[DEBUG:296] =======> backup = true, server state is backup");

      pico_stack_init();
      conf.setDev(init_picotcp());
      setup_server();

      if(conf.getMain_Heartbeats()){
        log_debug("[DEBUG:299] =======> main_heartbeats = true, backup will listen for ARPs");
      } else{
        log_debug("[DEBUG:299] =======> main_heartbeats = false, backup will initialize ARPs");
        heartbeat *hBeat = new heartbeat(conf.getIpv4_Addr(), conf.getDev(), conf.getHeartbeat_Timer());
        std::thread thd1 = hBeat->arp_checkThread();
      }

      pico_stack_loop();

    } else {
      log_debug("[DEBUG:304] =======> backup = false, server state is main");
      // Start the picoTcp stack
      pico_stack_init();
      log_debug("[DEBUG:307] =======> function call = pico_stack_init(), initalizing picoTCP IP-Stack");
      conf.setDev(init_picotcp());
      setup_server();

      if(conf.getMain_Heartbeats()){
        log_debug("[DEBUG:299] =======> main_heartbeats = true, main will initalize ARPs");
        heartbeat *hBeat = new heartbeat(conf.getBackup_Addr(), conf.getDev(), conf.getHeartbeat_Timer());
        std::thread thd1 = hBeat->arp_checkThread();
      } else{
        log_debug("[DEBUG:299] =======> main_heartbeats = false, main will listen for ARPs");
      }

        pico_stack_loop();
    }

    return 0;
}

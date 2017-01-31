#ifndef _CONFIG_PARSER_H_
#define _CONFIG_PARSER_H_

#include <json/json.h>
#include "json.hpp"
#include <iostream>
#include <string>

/* Description: This class is used to store the configuration read in from config.json
                for easy access later in the program. */

using json = nlohmann::json;
using namespace std;

class ConfigParser{
public:
  ConfigParser();
  bool getBackup();
  string getBackup_Addr();
  string getIpv4_Addr();
  int getPort();
  string getNetmask();
  int getHeartbeat_Timer();
  bool getDebug_Mode();
  string getTap_Device_name();
  bool getMain_Heartbeats();
  void setDev(struct pico_device *dev);
  struct pico_device *getDev();

private:
  bool backup;
  string backup_addr;
  string ipv4_addr;
  int port;
  string netmask;
  int heartbeat_timer;
  bool debug_mode;
  string tap_device_name;
  bool main_heartbeats;
  struct pico_device *dev;

  json read_config() {
      json res;
      ifstream myfile ("src/config.json");

      if (myfile.is_open())
      {
          string content( (std::istreambuf_iterator<char>(myfile) ),
                  (std::istreambuf_iterator<char>()    ) );
          res = json::parse(content);
          myfile.close();
          return res;
      }
      else {
          cout << "Unable to read config" << endl;
          return nullptr;
      }
  }

};

ConfigParser::ConfigParser(){
  json config = read_config();

  backup = config["backup"];
  backup_addr = config["backup_addr"];
  ipv4_addr = config["ipv4_addr"];
  port = config["port"];
  netmask = config["netmask"];
  heartbeat_timer = config["heartbeat_timer"];
  debug_mode = config["debug_mode"];
  tap_device_name = config["tap_device_name"];
  main_heartbeats = config["main_heartbeats"];
}

bool ConfigParser::getBackup(){
  return backup;
}

string ConfigParser::getBackup_Addr(){
  return backup_addr;
}

string ConfigParser::getIpv4_Addr(){
  return ipv4_addr;
}

string ConfigParser::getPort(){
  return port;
}

string ConfigParser::getNetmask(){
  return netmask;
}

int ConfigParser::getHeartbeat_Timer(){
  return heartbeat_timer;
}

bool ConfigParser::getDebug_Mode(){
  return debug_mode;
}

string ConfigParser::getTap_Device_name(){
  return tap_device_name;
}

bool ConfigParser::getMain_Heartbeats(){
  return main_heartbeats;
}

void setDev(struct pico_device *dev){
  this->dev = dev;
}
struct pico_device *getDev(){
  return dev;
}

#endif

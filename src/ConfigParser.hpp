#ifndef _CONFIG_PARSER_H_
#define _CONFIG_PARSER_H_

#include <json/json.h>
#include <iostream>
#include <string>

/* Description: This class is used to store the configuration read in from config.json
                for easy access later in the program. */
                
using json = nlohmann::json;
using namespace std;

class configParser{
public:
  configParser();
  bool getBackup();
  string getBackup_Addr();
  string getIpv4_Addr();
  string getNetmask();
  int getHeartbeat_Timer();
  bool getDebug_Mode();
  string getTap_Device_name();
  bool getMain_Heartbeats();

private:
  bool backup;
  string backup_addr;
  string ipv4_addr;
  string netmask;
  int heartbeat_timer;
  bool debug_mode;
  string tap_device_name;
  bool main_heartbeats;

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

configParser::configParser(){
  json config = read_config();

  backup = config["backup"];
  backup_addr = config["backup_addr"];
  ipv4_addr = config["ipv4_addr"];
  netmask = config["netmask"];
  heartbeat_timer = config["heartbeat_timer"];
  debug_mode = config["debug_mode"];
  tap_device_name = config["tap_device_name"];
  main_heartbeats = config["main_heartbeats"];
}

bool configParser::getBackup(){
  return backup;
}

string configParser::getBackup_Addr(){
  return backup_addr;
}

string configParser::getIpv4_Addr(){
  return ipv4_addr;
}

string configParser::getNetmask(){
  return netmask;
}

int configParser::getHeartbeat_Timer(){
  return heartbeat_timer;
}

bool configParser::getDebug_Mode(){
  return debug_mode;
}

string configParser::getTap_Device_name(){
  return tap_device_name;
}

bool configParser::getMain_Heartbeats(){
  return main_heartbeats;
}

#endif

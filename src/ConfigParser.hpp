#ifndef _CONFIG_PARSER_H_
#define _CONFIG_PARSER_H_

#include "json.hpp"
#include <iostream>
#include <string>

/* Description: This class is used to store the configuration read in from config.json
* for easy access later in the program.
* Author: Brice Aldrich, Devin Aspy, Zach Pratt
*/

using json = nlohmann::json; //using json namesapce for convieniance
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
  void setHwaddress(string hwaddress);
  string getHwaddress();
  void setDev(struct pico_device *device);
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
  string hwaddress;
  struct pico_device *dev;

  json read_config() { //code to read in configuration file, private because only needed internally.
      json res;
      ifstream myfile ("conf/config.json");

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

//Constructor: Let's read in our configuration and assign the values to the correct private fields.
ConfigParser::ConfigParser(){
  json config = read_config(); //read in config

  backup = config["backup"];
  backup_addr = config["backup_addr"];
  ipv4_addr = config["ipv4_addr"];
  port = config["port"];
  netmask = config["netmask"];
  heartbeat_timer = config["heartbeat_timer"];
  debug_mode = config["debug_mode"];
  tap_device_name = config["tap_device_name"];
  hwaddress = "";
}

bool ConfigParser::getBackup(){ //Getter for backup
  return backup;
}

string ConfigParser::getBackup_Addr(){ //Getter for backup_addr
  return backup_addr;
}

string ConfigParser::getIpv4_Addr(){ //getter for ipv4_addr
  return ipv4_addr;
}

int ConfigParser::getPort(){ //getter for port
  return port;
}

string ConfigParser::getNetmask(){ //getter for netmask
  return netmask;
}

int ConfigParser::getHeartbeat_Timer(){ //getter for heartbeat timer
  return heartbeat_timer;
}

bool ConfigParser::getDebug_Mode(){ //getter for debug_mode
  return debug_mode;
}

string ConfigParser::getTap_Device_name(){ //getter for tap_device_name
  return tap_device_name;
}

void ConfigParser::setHwaddress(string hwaddress){
  this-> hwaddress = hwaddress;
}

string ConfigParser::getHwaddress(){
  return hwaddress;
}

void ConfigParser::setDev(struct pico_device *device){ //setter for dev
  dev = device;
}
struct pico_device *ConfigParser::getDev(){ //getter for dev
  return dev;
}

#endif

#ifndef _CONFIG_H
#define _CONFIG_H

#include "main.h"

#include "FS.h"
//#include "SPIFFS.h"

//Define the datastucture passed between MQTT and ESP
class RadioSignal
{
public:
    unsigned long value;
    unsigned int proto;
    unsigned int length;
};
class AutoSwitch
{
public:
    boolean state;
    String start;
    String end;
    bool days[7] = {false};
    String sT;
    String eT;
    unsigned int sD;
    unsigned int eD;
    unsigned int onrange[2] = {10, 30};
    unsigned int offrange[2] = {60, 120};
    boolean irreg = false;
};
class SingleConfig
{
public:
    String name;
    boolean state;
    RadioSignal on;
    RadioSignal off;
    std::vector<AutoSwitch> autoSwitch;
    int needSend = 0;
};
extern std::vector<String> days_index;

extern std::vector<SingleConfig> configs;

//your wlan
extern String wlanSsid;
extern String wlanPassword;

//mqtt server to connect
extern String mqttFingerprint;
extern String mqttServer;
extern int mqttPort;
extern String mqttUser;
extern String mqttPassword;

//time zone offset
extern int timeZoneOffset;
extern boolean daylightSaving;
//for geolocation
extern float latitude;
extern float longitude;

//urls of api calls
extern String api_location;
extern String api_timezone;

//update server locations
extern String update_url;
//version tracking
extern String current_version;

bool config_setup();

#endif
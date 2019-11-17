#ifndef _WIFI_H
#define _WIFI_H


//wlan connection
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
extern WiFiClientSecure espClient;

//Time server connection
#include <NTPClient.h>
#include <WiFiUdp.h>
// Define NTP Client to get time
extern WiFiUDP ntpUDP;
extern NTPClient timeClient;
extern String wifiIP;
extern String wifiMAC;

bool wifi_setup();
void wifi_loop();
void update_timezone_offsets();
void wifi_check_ota_update();


#endif
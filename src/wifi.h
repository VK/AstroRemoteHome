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

//timiner for updating the firmware and the timezone
extern unsigned long wifi_next_ota_check;
extern unsigned long wifi_next_timezone_update;

bool wifi_setup();
void wifi_loop();
void update_timezone_offsets();
void update_time();
void wifi_check_ota_update();


#endif
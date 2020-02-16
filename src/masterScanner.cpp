#include "masterScanner.h"
#include "config.h"
#include "mqtt.h"
#include "wifi.h"
#include <ESP8266Ping.h>

unsigned int masterScanner_i;
unsigned int masterScanner_inteval;

bool masterScanner_setup() {
  masterScanner_i = 0;
  masterScanner_inteval = 0;
  return true;
}

void masterScanner_loop() {

  masterScanner_inteval++;
  if (masterScanner_inteval < 100) {
    // skip scanning
    return;
  }
  masterScanner_inteval = 0;

  unsigned long thisrun = timeClient.getEpochTime();

  // loop all master devices
  unsigned int i = masterScanner_i;

  // check if a new check is needed
  if (i < masterDevices.size() && masterDevices[i].nextCheck < thisrun) {
    Serial.print("# Ping ");
    Serial.print(masterDevices[i].IP.c_str());
    Serial.print(" ... ");

    // ping the device
    bool newState = Ping.ping(masterDevices[i].IP.c_str(), 3);

    if (newState) {
      Serial.println("online");
      // if the device is online send mqtt and schedule new scan
      masterDevices[i].online = true;
      masterDevices[i].lastSeen = thisrun;
      masterDevices[i].nextCheck = thisrun + 300ul;
      mqtt_master_update(masterDevices[i]);
    } else {

      if (masterDevices[i].online) {
        // if the device is offline for 10 minutes mark it as offline
        if (masterDevices[i].lastSeen + 600ul < thisrun) {
          Serial.println("offline");
          masterDevices[i].online = false;
          masterDevices[i].nextCheck = thisrun + 60ul;
          mqtt_master_update(masterDevices[i]);
        } else {
          Serial.println("perhaps offline");
          // check more often if the device just went offline
          // it might be because of some energy saving?
          masterDevices[i].nextCheck = thisrun + 60ul;
        }

      } else {
        // if the device is offline try to ping it again
        Serial.println("offline");
        masterDevices[i].online = false;
        masterDevices[i].nextCheck = thisrun + 300ul;
        mqtt_master_update(masterDevices[i]);
      }
    }
  }

  // scan another device the next time
  masterScanner_i++;
  if (masterScanner_i >= masterDevices.size()) {
    masterScanner_i = 0;
  }
}

bool masterScanner_online() {
  // this function returns true if a single device is online
  bool online = false;

  for (unsigned int i = 0; i < masterDevices.size(); i++) {

    if (masterDevices[i].online) {
      online = true;
    }
  }

  return online;
}
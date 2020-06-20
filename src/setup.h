#ifndef _SETUP_H
#define _SETUP_H

#include "main.h"

#include "wifi.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "FS.h"

void run_setup();
void update_config();


#endif
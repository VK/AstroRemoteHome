#include "masterScanner.h"
#include "mqtt.h"
#include "config.h"
#include "wifi.h"
#include <ESP8266Ping.h>


bool masterScanner_setup()
{
    return true;
}

void masterScanner_loop()
{
    unsigned long thisrun = timeClient.getEpochTime();

    //loop all master devices
    for (unsigned int i = 0; i < masterDevices.size(); i++)
    {

        //check if a new check is needed
        if (masterDevices[i].nextCheck < thisrun)
        {

            //ping the device
            bool newState = Ping.ping(masterDevices[i].IP.c_str(), 3);

            if (newState)
            {
                //if the device is online send mqtt and schedule new scan
                masterDevices[i].online = true;
                masterDevices[i].nextCheck = thisrun + 60ul;
                mqtt_master_update(masterDevices[i]);
            }
            else
            {

                if (masterDevices[i].online)
                {
                    //if the device is offline for 10 minutes mark it as offline
                    if (masterDevices[i].nextCheck + 300ul < thisrun)
                    {
                        masterDevices[i].online = false;
                        masterDevices[i].nextCheck = thisrun + 10ul;
                        mqtt_master_update(masterDevices[i]);
                    }
                }
                else
                {
                    //if the device is offline try to ping it again
                    masterDevices[i].nextCheck = thisrun + 10ul;
                }
            }

        }
    }
}

bool masterScanner_online()
{
    //this function returns true if a single device is online
    bool online = false;

    for (unsigned int i = 0; i < masterDevices.size(); i++)
    {

        if (masterDevices[i].online)
        {
            online = true;
        }
    }

    return online;
}
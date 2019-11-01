#ifndef _MQTT_H
#define _MQTT_H

#include "config.h"

//MQTT connection
#include <PubSubClient.h>
extern PubSubClient mqtt;

bool mqtt_setup();
void mqtt_publish(SingleConfig &cfg);

#endif
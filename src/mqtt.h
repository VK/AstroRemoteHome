#ifndef _MQTT_H
#define _MQTT_H

#include "config.h"

//MQTT connection
#include <PubSubClient.h>
extern PubSubClient mqtt;

bool mqtt_setup();
void mqtt_disconnect();
void mqtt_master_update(MasterDevice device);
void mqtt_publish(SingleConfig &cfg);
void mqtt_master_mode();
void mqtt_publish_config();

#endif
#ifndef _RADIO_H
#define _RADIO_H

#include "config.h"

#include <RCSwitch.h>
extern RCSwitch radio;

bool radio_setup();
void radio_loop();
void send_SingleConfig(SingleConfig cfg);

#endif
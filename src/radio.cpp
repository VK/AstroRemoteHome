#include "radio.h"
#include "mqtt.h"
#include "wifi.h"

RCSwitch radio = RCSwitch();
unsigned int radio_sendCooldown = 0;
unsigned int radio_sendId = 0;

bool radio_setup()
{
    radio.enableReceive(4); // Receiver on interrupt 4 => that is pin D2

    radio.enableTransmit(0); // Transmit on pin 0 => that is pin D3
    radio.setRepeatTransmit(2);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    return true;
}

void radio_loop()
{
    if (radio.available())
    {

        unsigned long value = radio.getReceivedValue();
        unsigned int proto = radio.getReceivedProtocol();
        unsigned int length = radio.getReceivedBitlength();

        bool found = false;

        //check if config needs to be updated because of a switching
        for (SingleConfig cfg : configs)
        {

            //internally switched off but switch on was sent
            if (cfg.state == false &&
                cfg.on.proto == proto &&
                cfg.on.length == length &&
                cfg.on.value == value)
            {
                cfg.state = true;
                found = true;
                mqtt_publish(cfg);
            }
            //internally switched on but switch off was sent
            if (cfg.state == true &&
                cfg.off.proto == proto &&
                cfg.off.length == length &&
                cfg.off.value == value)
            {
                cfg.state = false;
                found = true;
                mqtt_publish(cfg);
            }
        }

        if (!found)
        {
            String message = "[" + String(proto) + "," + String(length) + "," + String(value) + "]";
            mqtt.publish(("Esp/" + wifiMAC + "/received").c_str(), message.c_str());
        }

        radio.resetAvailable();
    }

    if (radio_sendCooldown == 0)
    {
        if (radio_sendId >= configs.size())
        {
            radio_sendId = 0;
        }
        if (radio_sendId < configs.size() && configs[radio_sendId].needSend > 0)
        {
            radio_sendCooldown = 100;
            configs[radio_sendId].needSend--;
            send_SingleConfig(configs[radio_sendId]);
        }
        radio_sendId++;
    }
    else
    {
        radio_sendCooldown--;
    }
}

void send_SingleConfig(SingleConfig cfg)
{
    if (cfg.state)
    {
        digitalWrite(LED_BUILTIN, LOW);
        radio.setProtocol(cfg.on.proto);
        radio.send(cfg.on.value, cfg.on.length);
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
        radio.setProtocol(cfg.off.proto);
        radio.send(cfg.off.value, cfg.off.length);
        digitalWrite(LED_BUILTIN, HIGH);
    }
}
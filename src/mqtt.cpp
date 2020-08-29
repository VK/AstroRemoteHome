#include "mqtt.h"
#include "config.h"
#include "wifi.h"
#include "radio.h"
#include "masterScanner.h"
#include "logic.h"

PubSubClient mqtt(espClient);
String clientId = "AstroRemote-";

/***
 * Parse a Json part and crate the AutoSwitch class
 * internally used to store switch on/off periods
 */
AutoSwitch getAutoSwitch(JsonVariant &value)
{
    AutoSwitch output = AutoSwitch();

    output.state = value["state"].as<bool>();
    output.start = value["start"].as<String>();
    output.end = value["end"].as<String>();
    output.sD = value["sD"].as<unsigned int>();
    output.eD = value["eD"].as<unsigned int>();
    output.sT = value["sT"].as<String>();
    output.eT = value["eT"].as<String>();

    if (value.containsKey("mode"))
    {
        output.mode = value["mode"].as<int>();
    }

    if (value.containsKey("irreg"))
    {
        output.irreg = value["irreg"].as<bool>();
        output.onrange[0] = value["onrange"][0].as<int>();
        output.onrange[1] = value["onrange"][1].as<int>();
        output.offrange[0] = value["offrange"][0].as<int>();
        output.offrange[1] = value["offrange"][1].as<int>();
    }

    JsonArray dayArray = value["days"].as<JsonArray>();
    for (JsonVariant dayValue : dayArray)
    {
        std::vector<String>::iterator pos = std::find(days_index.begin(), days_index.end(), dayValue.as<String>());
        int index = std::distance(days_index.begin(), pos);
        output.days[index] = true;
    }

    return output;
}

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{

    Serial.print("Message arrived in topic: ");
    Serial.println(topic);

    String message = "";
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    if (strcmp(topic, "Esp/send") == 0)
    {
        deserializeJson(doc, message);

        Serial.println("send");
        Serial.println(message);

        unsigned int proto = doc[0];
        unsigned int length = doc[1];
        unsigned long value = doc[2];

        radio.setProtocol(proto);
        radio.send(value, length);
    }

    //handle esp reboot
    if (strncmp(topic, ("Esp/" + wifiMAC + "/reboot").c_str(), 28) == 0)
    {
        ESP.restart();
    }

    //handle esp reboot
    if (strncmp(topic, ("Esp/" + wifiMAC + "/sendConfig").c_str(), 32) == 0)
    {
        mqtt_publish_config();
    }

    //handle master devices
    if (strncmp(topic, ("Esp/" + wifiMAC + "/Master/").c_str(), 29) == 0)
    {
        String newMasterIP = String(topic).substring(29);

        //delete a masterDevice
        if (strcmp(message.c_str(), "") == 0)
        {
            Serial.print("Delete Master Device ");
            Serial.println(newMasterIP);

            for (unsigned int i = 0; i < masterDevices.size(); i++)
            {
                if (masterDevices[i].IP == newMasterIP)
                {
                    masterDevices.erase(masterDevices.begin() + i);
                    return;
                }
            }
        }
        else
        {
            //check if master device already known

            bool found = false;
            for (unsigned int i = 0; i < masterDevices.size(); i++)
            {
                if (masterDevices[i].IP == newMasterIP)
                {
                    found = true;
                }
            }

            if (!found)
            {
                MasterDevice newObject = MasterDevice();
                newObject.IP = newMasterIP;
                masterDevices.push_back(newObject);
                Serial.print("Add new Master Device ");
                Serial.println(newMasterIP);
            }
        }
    }

    if (strncmp(topic, "MySockets/", 10) == 0)
    {
        //delete a socket
        if (strcmp(message.c_str(), "") == 0)
        {
            Serial.print("Delete Socket ");
            String delName = String(topic).substring(10);
            Serial.println(delName);

            for (unsigned int i = 0; i < configs.size(); i++)
            {
                if (configs[i].name == delName)
                {
                    configs.erase(configs.begin() + i);
                    return;
                }
            }
        }

        //switch on socket
        if (strcmp(message.c_str(), "ON") == 0)
        {
            String socketName = String(topic).substring(10);
            Serial.println(socketName);

            for (unsigned int i = 0; i < configs.size(); i++)
            {
                if (configs[i].name == socketName)
                {
                    if (configs[i].state == false)
                    {
                        configs[i].state = true;
                        configs[i].needSend = 15;
                        send_SingleConfig(configs[i]);
                        delay(5);
                        mqtt_publish(configs[i]);
                        return;
                    }
                }
            }
        }

        //switch off socket
        if (strcmp(message.c_str(), "OFF") == 0)
        {
            String socketName = String(topic).substring(10);
            Serial.println(socketName);

            for (unsigned int i = 0; i < configs.size(); i++)
            {
                if (configs[i].name == socketName)
                {
                    if (configs[i].state == true)
                    {
                        configs[i].state = false;
                        configs[i].needSend = 15;
                        send_SingleConfig(configs[i]);
                        delay(5);
                        mqtt_publish(configs[i]);
                        return;
                    }
                }
            }
        }

        deserializeJson(doc, message);
        String switchName = doc["name"];

        boolean found = false;
        for (unsigned int i = 0; i < configs.size(); i++)
        {
            if (configs[i].name == switchName)
            {
                bool oldState = configs[i].state;
                Serial.println("update config");
                found = true;
                configs[i].state = doc["state"].as<boolean>();

                //set the radio properties
                configs[i].on.value = doc["on"]["value"].as<unsigned long>();
                configs[i].on.length = doc["on"]["length"].as<unsigned int>();
                configs[i].on.proto = doc["on"]["protocol"].as<unsigned int>();
                configs[i].off.value = doc["off"]["value"].as<unsigned long>();
                configs[i].off.length = doc["off"]["length"].as<unsigned int>();
                configs[i].off.proto = doc["off"]["protocol"].as<unsigned int>();

                //update the switch properties
                configs[i].autoSwitch.clear();
                JsonArray inputArray = doc["auto"].as<JsonArray>();
                for (JsonVariant value : inputArray)
                {
                    configs[i].autoSwitch.push_back(getAutoSwitch(value));
                }

                if (oldState != configs[i].state)
                {
                    configs[i].needSend = 15;
                    send_SingleConfig(configs[i]);
                }
            }
        }

        if (!found)
        {
            SingleConfig cfg = SingleConfig();
            cfg.name = switchName;
            cfg.state = doc["state"].as<boolean>();

            //set the radio properties
            cfg.on = RadioSignal();
            cfg.on.value = doc["on"]["value"].as<unsigned long>();
            cfg.on.length = doc["on"]["length"].as<unsigned int>();
            cfg.on.proto = doc["on"]["protocol"].as<unsigned int>();
            cfg.off = RadioSignal();
            cfg.off.value = doc["off"]["value"].as<unsigned long>();
            cfg.off.length = doc["off"]["length"].as<unsigned int>();
            cfg.off.proto = doc["off"]["protocol"].as<unsigned int>();

            //set the switch properties
            cfg.autoSwitch = std::vector<AutoSwitch>();
            JsonArray inputArray = doc["auto"].as<JsonArray>();
            for (JsonVariant value : inputArray)
            {
                cfg.autoSwitch.push_back(getAutoSwitch(value));
            }

            configs.push_back(cfg);

            cfg.needSend = 15;
            send_SingleConfig(cfg);
        }
        Serial.println(message);
    }

    Serial.println();
    Serial.println("-----------------------");
}

bool mqtt_setup()
{

    if (mqttFingerprint != NULL && mqttFingerprint != "")
    {
        Serial.println("use fingerprint");
        espClient.setFingerprint(mqttFingerprint.c_str());
    }
    else
    {
        Serial.println("insecure!");
        espClient.setInsecure();
    }

    clientId = "AstroRemote-";
    clientId += WiFi.macAddress();

    mqtt.setServer(mqttServer.c_str(), mqttPort);
    mqtt.setCallback(mqtt_callback);

    int countdown = 10;
    Serial.print("MQTT ...");
    while (countdown > 0 && !mqtt.connected())
    {

        if (mqtt.connect(clientId.c_str(), mqttUser.c_str(), mqttPassword.c_str()), 0, 0, 0, 0, 0)
        {
            Serial.println(" connected.");
        }
        else
        {
            Serial.print(" failed with state ");
            Serial.println(mqtt.state());
        }
        countdown--;
    }
    if (!mqtt.connected())
    {
        return false;
    }

    mqtt.publish(("Esp/" + wifiMAC + "/Message").c_str(), "", true);
    mqtt_publish_config();

    mqtt.subscribe("MySockets/#");
    mqtt.subscribe("Esp/send");
    mqtt.subscribe(("Esp/" + wifiMAC + "/Master/#").c_str());
    mqtt.subscribe(("Esp/" + wifiMAC + "/reboot").c_str());
    mqtt.subscribe(("Esp/" + wifiMAC + "/sendConfig").c_str());
    return true;
}

void mqtt_publish_config()
{
    //publish mac and ip of this device
    mqtt.publish(("Esp/" + wifiMAC + "/IP").c_str(), wifiIP.c_str(), true);

    char PufferChar1[20];
    char PufferChar2[20];
    dtostrf(latitude, 1, 5, PufferChar1);
    dtostrf(longitude, 1, 5, PufferChar2);
    mqtt.publish(("Esp/" + wifiMAC + "/LOC").c_str(), ("{\"longitude\":" + String(PufferChar2) + ", \"latitude\":" + String(PufferChar1) + "}").c_str(), true);
    mqtt.publish(("Esp/" + wifiMAC + "/IP").c_str(), wifiIP.c_str(), true);
    mqtt.publish(("Esp/" + wifiMAC + "/VERS").c_str(), current_version.c_str(), true);
    mqtt.publish(("Esp/" + wifiMAC + "/LOC").c_str(), ("{\"longitude\":" + String(PufferChar2) + ", \"latitude\":" + String(PufferChar1) + "}").c_str(), true);

    //a recalculation of the logic will send sunrise and sunset information
    logic_trigger_recalc();
}

void mqtt_disconnect()
{
    mqtt.disconnect();
}

void mqtt_master_update(MasterDevice device)
{
    if (device.online)
    {
        mqtt.publish(("Esp/" + wifiMAC + "/Master/" + device.IP).c_str(), "online", true);
    }
    else
    {
        mqtt.publish(("Esp/" + wifiMAC + "/Master/" + device.IP).c_str(), "offline", true);
    }
}

void mqtt_master_mode()
{
    if (masterScanner_online())
    {
        mqtt.publish(("Esp/" + wifiMAC + "/MasterState").c_str(), "online", true);
    }
    else
    {
        mqtt.publish(("Esp/" + wifiMAC + "/MasterState").c_str(), "offline", true);
    }
}

void mqtt_publish(SingleConfig &cfg)
{
    doc.clear();

    doc["name"] = cfg.name;
    doc["state"] = cfg.state;
    doc["on"]["length"] = cfg.on.length;
    doc["on"]["protocol"] = cfg.on.proto;
    doc["on"]["value"] = cfg.on.value;
    doc["off"]["length"] = cfg.off.length;
    doc["off"]["protocol"] = cfg.off.proto;
    doc["off"]["value"] = cfg.off.value;

    JsonArray swArray = doc["auto"].to<JsonArray>();
    for (AutoSwitch sw : cfg.autoSwitch)
    {
        JsonObject val = swArray.createNestedObject();
        val["state"] = sw.state;
        val["start"] = sw.start;
        val["end"] = sw.end;
        val["sT"] = sw.sT;
        val["eT"] = sw.eT;
        val["sD"] = sw.sD;
        val["eD"] = sw.eD;
        val["mode"] = sw.mode;

        JsonArray onRange = val["onrange"].to<JsonArray>();
        onRange.add(sw.onrange[0]);
        onRange.add(sw.onrange[1]);
        JsonArray offRange = val["offrange"].to<JsonArray>();
        offRange.add(sw.offrange[0]);
        offRange.add(sw.offrange[1]);

        val["irreg"] = sw.irreg;
        JsonArray dayArray = val["days"].to<JsonArray>();
        for (int i = 0; i < 7; i++)
        {
            if (sw.days[i])
            {
                dayArray.add(days_index[i]);
            }
        }
    }

    char message[1000];
    serializeJson(doc, message);

    mqtt.publish(("MySockets/" + cfg.name).c_str(), message, true);
}

#include "setup.h"
#include "config.h"

bool setup_saveConfig = false;
void setSaveConfig()
{
    setup_saveConfig = true;
}

void run_setup()
{
    Serial.println("SETUP ");
    if (WiFi.isConnected())
    {
        WiFi.disconnect();
        Serial.println("disconnect to open portal");
        delay(1000);
    }

    WiFiManager wifiManager;

    wifiManager.resetSettings();
    wifiManager.setSaveConfigCallback(setSaveConfig);
    wifiManager.setConfigPortalTimeout(300);

    String oldWifiSSID = wlanSsid;
    String oldWifiPass = wlanPassword;
    String oldMqttPass = mqttPassword;
    String old_api_location = api_location;
    String old_api_timezone = api_timezone;
    String old_update_url = update_url;

    WiFiManagerParameter setup_mqttServer("mqttServer", "mqttServer", mqttServer.c_str(), 99);
    wifiManager.addParameter(&setup_mqttServer);
    WiFiManagerParameter setup_mqttPort("mqttPort", "mqttPort", String(mqttPort).c_str(), 9);
    wifiManager.addParameter(&setup_mqttPort);
    WiFiManagerParameter setup_mqttUser("mqttUser", "mqttUser", mqttUser.c_str(), 99);
    wifiManager.addParameter(&setup_mqttUser);
    WiFiManagerParameter setup_mqttPassword("mqttPassword", "mqttPassword", "", 99);
    wifiManager.addParameter(&setup_mqttPassword);
    WiFiManagerParameter setup_mqttFingerprint("mqttFingerprint", "mqttFingerprint", mqttFingerprint.c_str(), 99);
    wifiManager.addParameter(&setup_mqttFingerprint);

    //geolocation
    char PufferChar1[20];
    char PufferChar2[20];
    dtostrf(latitude, 1, 5, PufferChar1);
    dtostrf(longitude, 1, 5, PufferChar2);
    WiFiManagerParameter setup_latitude("latitude", "latitude", PufferChar1, 20);
    wifiManager.addParameter(&setup_latitude);
    WiFiManagerParameter setup_longitude("longitude", "longitude", PufferChar2, 20);
    wifiManager.addParameter(&setup_longitude);

    wifiManager.setBreakAfterConfig(true);
    wifiManager.autoConnect("AstroRemoteHome");

    //lets hope all parameters are right and we use the data to write a config file
    //if something is wrong we will see if we and up in the config module again
    if (true || setup_saveConfig)
    {

        if (SPIFFS.begin())
        {
            Serial.println("SPIFFS.begin() success");
        }
        else
        {
            Serial.println("SPIFFS.begin() error!");
            return;
        }

        File file = SPIFFS.open("/config.json", "w");

        doc.clear();
        if (WiFi.SSID() != "")
        {
            doc["wlan"]["ssid"] = WiFi.SSID();
        }
        else
        {
            doc["wlan"]["ssid"] = oldWifiSSID;
        }

        if (WiFi.psk() != "")
        {
            doc["wlan"]["password"] = WiFi.psk();
        }
        else
        {
            doc["wlan"]["password"] = oldWifiPass;
        }

        doc["mqtt"]["fingerprint"] = setup_mqttFingerprint.getValue();
        doc["mqtt"]["server"] = setup_mqttServer.getValue();
        doc["mqtt"]["port"] = atoi(setup_mqttPort.getValue());
        doc["mqtt"]["user"] = setup_mqttUser.getValue();
        if (strcmp(setup_mqttPassword.getValue(), "") != 0)
        {
            doc["mqtt"]["password"] = setup_mqttPassword.getValue();
        }
        else
        {
            doc["mqtt"]["password"] = oldMqttPass;
        }

        doc["loc"]["latitude"] = atof(setup_latitude.getValue());
        doc["loc"]["longitude"] = atof(setup_longitude.getValue());

        doc["api"]["location"] = old_api_location;
        doc["api"]["timezone"] = old_api_timezone;

        doc["update"]["url"] = old_update_url;

        char message[1000];
        serializeJson(doc, message);
        Serial.println(message);

        if (serializeJson(doc, file) == 0)
        {
            Serial.println(F("Failed to write to file"));
        }

        // Close the file
        file.close();
    }

    Serial.println("Ferig");

    delay(1000);

    ESP.restart();
}

void update_config()
{
    //put some config update routines here
    if (false)
    {

        doc["api"]["location"] = "???";
        doc["api"]["timezone"] = "???";

        Serial.print("update needed\nnew config:\n\n");
        char message[1000];
        serializeJson(doc, message);
        Serial.println(message);
        Serial.print("\nwrite file\n");

        File file = SPIFFS.open("/config.json", "w");
        if (serializeJson(doc, file) == 0)
        {
            Serial.println(F("Failed to write to file"));
            file.close();
        }
        else
        {
            Serial.println("reboot with new config");
            file.close();
            delay(1000);

            ESP.restart();
        }
    }
}
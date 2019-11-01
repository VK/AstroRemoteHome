#include "wifi.h"
#include "config.h"
#include "mqtt.h"
#include "TimeLib.h"
#include "ESP8266HTTPClient.h"

WiFiClientSecure espClient;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 0, 60000);
String wifiIP;
String wifiMAC;
HTTPClient http;

unsigned int connection_error_count = 0;
unsigned int keepalive_send_loop = 0;

bool wifi_setup()
{
    WiFi.begin(wlanSsid.c_str(), wlanPassword.c_str());

    Serial.print("WiFi ");
    int countdown = 60;
    while (WiFi.status() != WL_CONNECTED && countdown > 0)
    {
        delay(500);
        Serial.print(".");
        countdown--;
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(" no connection possible");
        return false;
    }
    else
    {
        Serial.println(" connected.");
    }

    wifiIP = WiFi.localIP().toString();
    wifiMAC = WiFi.macAddress();

    timeClient.begin();

    if (latitude == 0.0 && longitude == 0.0)
    {
        //the location is usually fixed in the config file.
        //If not, we use a geolocation here to get an approximate value

        //get the location
        http.begin(api_location);
        int httpCode = http.GET();

        // httpCode will be negative on error
        if (httpCode > 0)
        {
            String message = http.getString();
            Serial.println(message);
            deserializeJson(doc, message);

            if (doc.containsKey("latitude") && doc.containsKey("longitude"))
            {
                latitude = doc["latitude"].as<float>();
                longitude = doc["longitude"].as<float>();

                Serial.println("approximate location:");
                Serial.println(longitude);
                Serial.println(latitude);
            }
        }
    }

    return true;
}

void update_timezone_offsets()
{

    //get the location
    http.begin(api_timezone + "&by=position&lat=" + String(latitude) + "&lng=" + String(longitude) + "&format=json");
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
        String message = http.getString();
        Serial.println(message);
        deserializeJson(doc, message);

        if (doc.containsKey("gmtOffset") && doc.containsKey("dst"))
        {
            timeZoneOffset = doc["gmtOffset"].as<int>() / 60;
            daylightSaving = (doc["dst"].as<int>() == 1);
            Serial.print("time   ");
            Serial.print(timeZoneOffset);
            Serial.println(daylightSaving);
        }
    }
}

void wifi_loop()
{
    keepalive_send_loop++;
    if (WiFi.status() == WL_CONNECTED)
    {
        timeClient.update();
        setTime(timeClient.getEpochTime());

        if (mqtt.connected())
        {
            mqtt.loop();
            connection_error_count = 0;

            if (keepalive_send_loop > 10000)
            {
                //publish mac and ip of this device
                mqtt.publish(("Esp/" + wifiMAC + "/IP").c_str(), wifiIP.c_str(), true);
                keepalive_send_loop = 0;
            }
        }
        else
        {
            Serial.println("MQTT connection error");
            connection_error_count++;
            delay(100);
        }
    }
    else
    {
        Serial.println("WiFi connection error");
        mqtt.disconnect();
        connection_error_count++;
        delay(100);
    }

    if (connection_error_count == 6)
    {
        mqtt.publish(("Esp/" + wifiMAC + "/Message").c_str(), "Connection Errors!", true);
    }
    if (connection_error_count > 60)
    {
        ESP.restart();
    }
}
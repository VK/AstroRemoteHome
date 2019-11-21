#include "wifi.h"
#include "config.h"
#include "mqtt.h"
#include "TimeLib.h"
#include "ESP8266HTTPClient.h"
#include "ESP8266httpUpdate.h"

WiFiClientSecure espClient;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 0, 60000);
String wifiIP;
String wifiMAC;
HTTPClient http;

unsigned int connection_error_count = 0;
unsigned int keepalive_send_loop = 0;
unsigned long wifi_next_ota_check;

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

    //init ota update start time, check for new updates in 10 Minutes
    timeClient.update();
    update_time();
    
    wifi_next_ota_check = timeClient.getEpochTime() + 600ul;

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

        update_time();
    }
}


void update_time()
{
    unsigned long thisrun = timeClient.getEpochTime();
    thisrun += timeZoneOffset * SECS_PER_MIN;

    if (daylightSaving)
    {
        thisrun += SECS_PER_HOUR;
    }

    setTime(thisrun);
}

void wifi_loop()
{
    keepalive_send_loop++;
    if (WiFi.status() == WL_CONNECTED)
    {
        timeClient.update();
        update_time();

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
            delay(1000);
            mqtt_setup();
        }

        wifi_check_ota_update();
    }
    else
    {
        Serial.println("WiFi connection error");
        mqtt.disconnect();
        connection_error_count++;
        delay(1000);
        wifi_setup();
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

void wifi_check_ota_update()
{
    unsigned long thisrun = timeClient.getEpochTime();

    if (thisrun > wifi_next_ota_check)
    {
        mqtt_disconnect();
        delay(100);
        Serial.println("Check for new firmware");

        auto ret = ESPhttpUpdate.update(espClient, update_url, current_version);
        // if successful, ESP will restart

        switch (ret)
        {
        case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;

        case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            break;
        }

        //check for new updates the next day
        wifi_next_ota_check = thisrun + 86400ul;
    }
}
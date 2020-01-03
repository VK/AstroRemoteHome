#include "config.h"

//the config vector
std::vector<SingleConfig> configs;
//way to order the days of a week
std::vector<String> days_index = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

//variable to store the current version
String current_version = "1.0_2020.01.03A";

//your wlan
String wlanSsid;
String wlanPassword;

//mqtt server to connect
String mqttFingerprint;
String mqttServer;
int mqttPort;
String mqttUser;
String mqttPassword;

//timezone offset
int timeZoneOffset = 60;
boolean daylightSaving = false;
//geolocation
float latitude;
float longitude;

//api locations
String api_location;
String api_timezone;

//update location
String update_url;

bool config_setup()
{
    Serial.print("Current Version: ");
    Serial.println(current_version);
    Serial.println("Init Config");

    //Initialize File System
    if (SPIFFS.begin())
    {
        Serial.println("SPIFFS.begin() success");
    }
    else
    {
        Serial.println("SPIFFS.begin() error!");
        return false;
    }

    File file = SPIFFS.open("/config.json", "r");

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error)
    {
        Serial.println(F("Failed to read file, using default configuration"));
        return false;
    }

    wlanSsid = doc["wlan"]["ssid"].as<String>();
    wlanPassword = doc["wlan"]["password"].as<String>();

    String mqttFingerprint = doc["mqtt"]["fingerprint"];
    mqttServer = doc["mqtt"]["server"].as<String>();
    mqttPort = doc["mqtt"]["port"].as<int>();
    mqttUser = doc["mqtt"]["user"].as<String>();
    mqttPassword = doc["mqtt"]["password"].as<String>();


    if (doc.containsKey("loc") &&
        doc["loc"].containsKey("latitude") &&
        doc["loc"].containsKey("longitude"))
    {
        latitude = doc["loc"]["latitude"].as<float>();
        longitude = doc["loc"]["longitude"].as<float>();
    }
    else
    {
        latitude = 0.0;
        longitude = 0.0;
    }

    api_location = doc["api"]["location"].as<String>();
    api_timezone = doc["api"]["timezone"].as<String>();

    update_url = doc["update"]["url"].as<String>();

    char message[1000];
    serializeJson(doc, message);
    Serial.println(message);

    Serial.println(mqttServer);
    Serial.println(mqttPort);
    Serial.println(mqttUser);
    Serial.println(mqttPassword);
    Serial.println(mqttFingerprint);

    return true;
}
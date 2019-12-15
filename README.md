# AstroRemoteHome

AstroRemoteHome is an Esp8266 Arduino project to remote controll 433 MHz sockets.
Each one can be individually controlled base on a local time or relative to sunrise and sunset.
All required time information is retrieved from remote services and the switch settings are exchanged via an MQTT.

The web app to control the setup is located at [vk.github.io/AstroRemoteHome/](https://vk.github.io/AstroRemoteHome/)











# Setup
## Microcontroller Setup

* ESP 8266
* RF Link Transmitter - 434MHz eg. [Sparkfun](https://www.sparkfun.com/products/10534)
* RF Link Receiver - 434MHz [Sparkfun](https://www.sparkfun.com/products/10532)

![Breadboard](https://vk.github.io/AstroRemoteHome/img/breadboard.png "BreadBoard")

Controller box                                               | Web App in action
:---------------------------------------------------------------:|:-----------------------------------------------------------:
<img src="https://vk.github.io/AstroRemoteHome/img/box_open.png" height="375px" width="375px" /><br/><img src="https://vk.github.io/AstroRemoteHome/img/box_closed.png" height="375px" width="375px" />  |  ![](https://vk.github.io/AstroRemoteHome/img/app.gif)


## Configuration

The configuration of the microcontroller is stored in the config.json file in SPIFFS and holds a section for your WLAN, MQTT server, geolocation and api calls.
The settings can be changed via a webinterface, however it's much easier to upload an working initial config file.

```javascript
{
    "wlan": {
        "ssid": "",
        "password": ""
    },
    "mqtt": {
        "fingerprint": "",
        "server": "",
        "port": 8883,
        "user": "",
        "password": ""
    },
    "loc": {
        "latitude": 0,
        "longitude": 0
    },
    "api": {
        "location": "http://api.ipstack.com/check?access_key=???",
        "timezone": "http://api.timezonedb.com/v2.1/get-time-zone?key=???"
    },
    "update": {
        "url": ""
    }
}
```

The current version uses two remote services to collect the geolocation of the device and the current timezone after connecting to the local WLAN.
* [IPstack geoloaction](https://ipstack.com/)
* [Timezone DB](https://timezonedb.com/)

You need to create an account to aquire your individual api keys to finish the configuration



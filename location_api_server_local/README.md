# AstroRemoteHome - Api Server local

![Docker Image location-api-server-local](https://github.com/VK/AstroRemoteHome/workflows/Docker%20Image%20vikru/location-api-server-local/badge.svg)

The current version uses two remote services to collect the geolocation of the device and the current timezone after connecting to the local WLAN.

If you want to run your own local server you can use this reimplementation.

> Note! The geolocation only delivers one location, and the timezone offset is just based on the local location.


## Run Server
```
pipenv install
pipenv shell
python main.py
```

## Docker Container
```
docker build -t vikru/location-api-server-local .
docker run -p 80:80 -t vikru/location-api-server-local
```
# AstroRemoteHome - Location Api Server

The current version uses two remote services to collect the geolocation of the device and the current timezone after connecting to the local WLAN.

If you want to run your own server you can use this reimplementation.

> Note! Add your version of **[GeoLite2-City.mmdb](https://dev.maxmind.com/geoip/geoip2/geolite2/)** 

## Run Server
```
pipenv install
pipenv shell
python main.py
```


## Docker Container
```
docker build -t vikru/location-api-server .
docker run -p 80:80 -t vikru/location-api-server
```



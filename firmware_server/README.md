# AstroRemoteHome - Firmware Server

A simple server to distribute new firmware versions to the microcontrollers.

## Docker Container
```
docker build -t vikru/astroremotehome-firmware-server .
docker run -p 80:80 -t vikru/astroremotehome-firmware-server
```
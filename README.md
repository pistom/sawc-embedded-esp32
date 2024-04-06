# Hardware
Diagrams will be added soon.

# Instalation

Install PlatformIO on Visual Studio Code.

## Install libraries
- marcoschwartz/LiquidCrystal_I2C@^1.1.4
- wnatth3/WiFiManager@^2.0.16-rc.2
- fhessel/esp32_https_server_compat@^1.0.0

## Generate ssl certificate
Create a directory to store the certificate
```bash
cd src
mkdir cert
cd cert
../create_cert.sh
```

## Define devices outputs pins
In `AppConfig.h` change the `valvesNumber` value to the number of valves you have. 
In `AppConfig.cpp` change the `valvesGpioPins` array to the GPIO pins you are using to control the valves.

## Configure LCD if you have one
In `devices/lcd.cpp` define I2C address and display size.
Search for "ESP32: I2C Scanner" to find the I2C address of your display.

# Running
Upload and monitor code using PlatformIO.
For the first run the device will create a WiFi network. SSID and password will be displayed on the LCD and serial monitor. Connect to the network and configure the connection to your WiFi network.

After that, the device will connect to your WiFi network and you can access it through the IP address displayed on the LCD and serial monitor.

On the LCD you will see the token you should use to access the API.

# API
You can open and close the valves and the pump using the API. The API is available at `http://<device_ip>/`.

## Endpoints
- `/output` - POST - Open or close a valve or the pump.
Example of raw body:

```
token=<token>&output=<number>&action=<action>&duration=<duration>&pumpDelayOff=<seconds>&nextOutput=<number>
```

- `token` - The token displayed on the LCD.
- `output` - The number of the output you want to control. For controlling the pump use `pump`.
- `action` - `on` or `off`.
- `duration` - The time in seconds the output will be on.
- `pumpDelayOff` - The time in seconds the pump will be off before the output is turned off (to prevent the pump from turning on when the valve is closed).
- `nextOutput` - The number of the output that will be turned on after the current output is turned off (to prevent delays between outputs).
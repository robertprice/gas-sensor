# Gas Sensor
This is the prototype gas sensor used for [Clean Air Eastbourne](https://eastbourneair.com/).

It uses a [Grove multichannel gas sensor](https://coolcomponents.co.uk/products/grove-multichannel-gas-sensor-co-no2-h2-nh3-ch4) (which is based on the MiCS-6814), a [BME280](https://www.aliexpress.com/item/1PCS-Digital-Temperature-Humidity-Barometric-Pressure-Sensor-Module-Breakout-BME280-SPI-and-I2C-interface/32795463531.html) for temperature / humidity / air pressure, and [NodeMCU](https://www.aliexpress.com/item/10PCS-New-version-Wireless-module-CH340-NodeMcu-V3-Lua-WIFI-Internet-of-Things-development-board-based/32656786959.html?) microcontroller.

Both the Grove and BME280 use I2C to talk to the NodeMCU. The NodeMCU then posts the data in JSON format to a web service.

The code also uses mDNS to create an entry "gassensor.local" that can be pinged on your local network to check that the device has connected successfully.

# Code
The code can be found in the `src/gassensor` directory.

Change `settings.h` to include your own WiFi settings

```cpp
#define WIFI_Network "Your network name"
#define WIFI_Password "Your network password"
```

# Wiring
![Wiring the gas sensor](https://github.com/robertprice/gas-sensor/blob/master/docs/images/wiring-diagram.png?raw=true)

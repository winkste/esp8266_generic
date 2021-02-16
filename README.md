# C++ / Arduino based ESP8266 application
This project integrates several different devices into one application. The target for this application is a ESP8266 microcontroller. The programming language used is C++. The system is devided in:
- driver layer
- support and protocol modules
- device layer

## The driver layer includes handling for:
- gpio abstraction (MCP, ESP internal)
- parameter handling using WIFIManager
- application information handling
- trace message and debug logging interface

## The support and protocol modules include:
- over the air firmware updates using configurations in platformio
```
upload_protocol = espota
```
- mqtt abstraction handling including subscriber and publisher modules
- device manager for starting, executing and stopping the devices

## Application Devices
- BME280
- DHT22
- Dimmable Light
- Neo Pixels
- PIR
- Power Save
- SEN0193
- SingleRelay
- Sonoff Basic
- Temt6000

### Device Configurations
- CAPABILITY_MULTI_SENSE_RELAY    0x14u
- CAPABILITY_3D_PRINTER           0x13u
- CAPABILITY_NEOPIXELS            0x12u
- CAPABILITY_H801                 0x11u
- CAPABILITY_DIM_LIGHT            0x10u
- CAPABILITY_MULTI_SENSE          0x0Fu
- CAPABILITY_MOISTURE_ONLY        0x0Eu
- CAPABILITY_SONOFF_PIR           0x0Du
- CAPABILITY_EIGHT_RELAY_ESP      0x0Cu
- CAPABILITY_DOUBLE_DHT           0x0Bu
- CAPABILITY_BME_SENSOR           0x0Au
- CAPABILITY_FOUR_RELAY_MCP       0x09u
- CAPABILITY_FOUR_RELAY           0x08u
- CAPABILITY_PIR_RELAY            0x05u
- CAPABILITY_DHT_SENSOR_BAT       0x04u
- CAPABILITY_PIR                  0x03u
- CAPABILITY_SONOFF_BASIC         0x02u
- CAPABILITY_DHT_SENSOR           0x01u
- CAPABILITY_SINGLE_RELAY         0x00u

## Setup & Preparations

### WIFIManager configuration:
TBD

### Development Environment
- IDE: VSCode with PlatformIO extension installed

### Platform Settings

```
platform = espressif8266
framework = arduino
```

### Serial monitor speed

```
monitor_speed = 115200
```

### Targets


- d1_mini
- nodemcu
- esp8285 (SONOFF)


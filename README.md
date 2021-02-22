ArduinoEnergyLogger
===========

An arduino code that reads Current Transformer (CT sensor) data, SRNE Mppt controller data, and SNAT PSW inverter.
The data collected will be transmit to a HTTP server via POST with a CSV formatted data.
The device used in this project is Wemos D1 mini.

Please refer to the screenshots below.

Applications
--------
* Solar and Energy Datalogging
* Diagnostics
* Consumption Analysis


Supports
--------
* CT Sensor
* SRNE Mppt Controller
* SNAT PSW Inverter


Files
-----
* ArduinoEnergyLogger.ino - main file
* ct.ino - support file for ct sensor reading (based on emon).
* display.ino - support file for displaying data via LCD.
* post.ino - support file for posting data to a HTTP server.
* srne.ino - support file for extracting data from a srne mppt controller.
* snat.ino - support file for extracting data from a snat psw inverter.
* webserver.ino - support file for the device webserver, used for wifi and device configurations/informations.
* wifi.ino - support file for wifi.
* utils.ino - helpers.


Wiring for CT sensor, SRNE, and SNAT
--------------------
    CT sensor                 Wemos D1 mini
    divider circuit --------- ADC0 (A0)
    (2 x 100k ohms and 10uF)
    
    SRNE                      Wemos D1 mini
    Rx ---------------------- D7
    Tx ---------------------- D8

    SRNE                      Wemos D1 mini
    Rx ---------------------- D5
    Tx ---------------------- D6


Screenshots (Grafana display)
---------------

![Alt text](https://github.com/kerpz/ArduinoEnergyLogger/blob/main/screenshots/grafana-1.png "Page 1")
![Alt text](https://github.com/kerpz/ArduinoEnergyLogger/blob/main/screenshots/grafana-2.png "Page 2")

Screenshots (Device Webserver)
---------------

![Alt text](https://github.com/kerpz/ArduinoEnergyLogger/blob/main/screenshots/Screenshot_20210218-212110_Chrome.jpg "Main Page")
![Alt text](https://github.com/kerpz/ArduinoEnergyLogger/blob/main/screenshots/Screenshot_20210218-212210_Chrome.jpg "Information 1")
![Alt text](https://github.com/kerpz/ArduinoEnergyLogger/blob/main/screenshots/Screenshot_20210218-212221_Chrome.jpg "Information 2")
![Alt text](https://github.com/kerpz/ArduinoEnergyLogger/blob/main/screenshots/Screenshot_20210218-212327_Chrome.jpg "Configuration 1")
![Alt text](https://github.com/kerpz/ArduinoEnergyLogger/blob/main/screenshots/Screenshot_20210218-212343_Chrome.jpg "Configuration 3")


NOTES
-----
* Still in beta test

TODO
-----
* Device mass production

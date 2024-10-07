/*
 Author:
 - Philip Bordado (kerpz@yahoo.com)

 Hardware:
 - Wemos D1 mini (Compatible board)

 Software:
 - Arduino 2.2.1 (Stable)
 - vscode-arduino (https://github.com/vscode-arduino/vscode-arduino)
 - Board 3.1.2
 - Adafruit SSD1306 2.5.7
 - Adafruit ADS1115 2.4.0
 - NTPClient 3.2.1
 - ArduinoJson 6.21.3
*/

#include "eeprom.h"
#include "network.h"
#include "webserver.h"
#include "app.h"

void setup()
{
  delay(10);

  Serial.begin(115200);

  Serial.println();
  Serial.println(APPNAME);

  loadConfig();

  networkSetup();
  webserverSetup();

  appSetup();

  delay(100);
}

void loop()
{
  networkLoop();
  webserverLoop();

  appLoop();
}

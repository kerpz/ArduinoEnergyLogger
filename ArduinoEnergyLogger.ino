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
#include "ntp.h"
#include "display.h"
#include "analog.h"
#include "webserver.h"
#include "post.h"
#include "app.h"

void execEvery(int ms)
{
  static unsigned long msTick = millis();
  static uint8_t sTick;

  if (millis() - msTick >= ms)
  { // run every N ms
    msTick = millis();

    ntpLoop();
    // Serial.println(epoch);

    if (sTick >= 59)
    {
      sTick = 0;
      run_time++;
      if (post_enable)
        postLoop();
    }
    else
    {
      sTick++;
    }
  }
}

void setup()
{
  delay(10);

  Serial.begin(115200);

  Serial.println();
  Serial.println(APPNAME);

  loadConfig();

  networkSetup();
  ntpSetup();
  webserverSetup();

  // if (beep_enable)
  //  beepSetup();
  if (analog_enable)
    analogSetup();
  if (display_enable)
    displaySetup();
  // if (ads1115_enable)
  //  ads1115Setup();

  appSetup();

  delay(100);
}

void loop()
{
  execEvery(1000);

  if (display_enable)
    displayLoop();
  // if (dht11_enable)
  //  dht11Loop();

  networkLoop();
  webserverLoop();

  appLoop();
}

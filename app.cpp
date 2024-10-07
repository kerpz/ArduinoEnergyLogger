// wifi
#include <ESP8266WiFi.h>

#include "app.h"

// https://www.tinkercad.com/things/dJn6LkhyR5E-interface-test

uint16_t run_time = 0;

// timing
int timezone = 0;
unsigned long epoch = 0;
uint8_t second = 0;
uint8_t minute = 0;
uint8_t hour = 0;
uint8_t day = 0;
uint8_t month = 0;
uint16_t year = 0;
char datetime[32] = "";

static bool action = false;

void appSetup()
{
  ntpSetup();
  // srne @ D7 D8
  // snat @ D5 D6
  if (display_enable)
    displayLoop();
  // if (dht11_enable)
  //  dht11Loop();

  // if (beep_enable)
  //  beepSetup();
  if (analog_enable)
    analogSetup();
  if (display_enable)
    displaySetup();
  // if (ads1115_enable)
  //  ads1115Setup();
  if (srne_enable)
    srneSetup();
  if (snat_enable)
    snatSetup();
}

void appLoop()
{
  // static float _prev_device_voltage;
  static uint32_t msTick = millis();
  // static uint8_t sTick;

  if (millis() - msTick >= 1000) // 1000ms refresh rate
  {
    msTick = millis();

    ntpLoop();

    if (analog_enable)
      analogLoop();
    delay(1);
    if (srne_enable)
      srneLoop();
    delay(1);
    if (snat_enable)
      snatLoop();

    if (second >= 59)
    {
      if (post_enable)
        postLoop();
      second = 0;

      /*
      if (minute >= 59)
      {
        minute = 0;
        if (hour >= 23)
        {
          hour = 0;
        }
        else
          hour++;
      }
      else
        minute++;
        */
    }
    else
      second++;
  }
}
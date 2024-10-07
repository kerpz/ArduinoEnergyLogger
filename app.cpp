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
  // srne @ D7 D8
  // snat @ D5 D6
}

void appLoop()
{
  // static float _prev_device_voltage;
  static uint32_t msTick = millis();
  static uint16_t alarm_duration;

  if (millis() - msTick >= 1000) // 1000ms refresh rate
  {
    msTick = millis();

    if (srne_enable)
      srneLoop();
    if (snat_enable)
      snatLoop();
  }
}
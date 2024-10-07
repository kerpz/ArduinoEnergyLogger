#ifndef APP_H
#define APP_H

#include "eeprom.h"
#include "analog.h"
#include "post.h"
#include "srne.h"
#include "snat.h"

#define APPNAME "EnergyLogger v1.0"

#define APssid "EnergyLogger-AP"
#define APpassword "12345678"

extern float dht11_temperature;
extern float dht11_humidity;

extern uint16_t run_time;

// timing
extern int timezone;
extern unsigned long epoch;
extern uint8_t second;
extern uint8_t minute;
extern uint8_t hour;
extern uint8_t day;
extern uint8_t month;
extern uint16_t year;
extern char datetime[32];

void appSetup();
void appLoop();

#endif
#ifndef SRNE_H
#define SRNE_H

// #include "analog.h"
#include "app.h"

extern uint16_t modbus_error;

// SRNE Modbus part
extern float battery_voltage;
extern float battery_charge;
extern float pv_voltage;
extern float pv_power;
extern float mppt_voltage;
extern float mppt_power;
extern float battery_temperature;
extern float mppt_temperature;
extern float dc_voltage;
extern float dc_current;
extern float dc_power;

void srneSetup();
void srneLoop();

#endif
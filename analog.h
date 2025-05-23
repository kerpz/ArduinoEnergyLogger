#ifndef ANALOG_H
#define ANALOG_H

extern float ct_voltage;
extern float ct_current;
extern float ct_power;

void analogSetup();
void analogLoop();

#endif
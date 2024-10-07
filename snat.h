#ifndef SNAT_H
#define SNAT_H

// #include "analog.h"
#include "app.h"

extern uint16_t megatec_error;

extern float inv_in_voltage;
extern float inv_in_fault_voltage;
extern float inv_out_voltage;
extern float inv_out_power;
extern float inv_in_frequency;
extern float inv_temperature;
extern float inv_cell_voltage;
extern char inv_flags[9];

void snatSetup();
void snatLoop();

#endif
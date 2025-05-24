#ifndef POST_H
#define POST_H

extern uint16_t post_error;
extern uint16_t wifi_error;

#include "eeprom.h"

void postData(const String &data);

#endif
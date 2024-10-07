#ifndef POST_H
#define POST_H

extern uint16_t post_error;
extern uint16_t wifi_error;

// #include "analog.h"
#include "app.h"

void postData(const String &data);
void postLoop();

#endif
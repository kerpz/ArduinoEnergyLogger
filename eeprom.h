#ifndef EEPROM_H
#define EEPROM_H

extern char ssid[32];
extern char password[32];

// peripheral devices
extern uint8_t beep_enable;
extern uint8_t analog_enable;
extern uint8_t display_enable;
extern uint8_t ads1115_enable;

extern uint8_t post_enable;
extern char api_key[32];
extern char api_url[256];
extern uint16_t http_timeout;

// emon
extern uint8_t ct_enable;
extern float ct_calibration;
extern float ct_pf;
extern float ct_voltage;

extern uint8_t srne_enable;
extern uint8_t snat_enable;

void loadConfig();
void saveConfig();

#endif
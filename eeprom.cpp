#include <Arduino.h>
// eeprom
#include <EEPROM.h>

#include "eeprom.h"

// globals
char ssid[] = "KERPZ-AP2";     // limit to 32 bytes
char password[] = "hackmelol"; // limit to 32 bytes

uint8_t beep_enable = 0;
uint8_t analog_enable = 1;
uint8_t display_enable = 0;
uint8_t ads1115_enable = 0;

uint8_t post_enable = 0;
char api_key[] = "NIJCG7UI28O9CAYD";                                       // limit to 32
char api_url[] = "https://192.168.2.1:8001/cgi-bin/custom-full.cgi?a=iot"; // limit to 256
uint16_t http_timeout = 0;

// emon
uint8_t ct_enable = 1;
float ct_calibration = 30.0;
float ct_pf = 1.0;
float ct_voltage = 235.0;

uint8_t srne_enable = 1;
uint8_t snat_enable = 0;

void loadConfig()
{
  EEPROM.begin(512);

  char ok[3];
  EEPROM.get(386, ok);

  if (String(ok) == String("OK"))
  {
    Serial.print("Loading Storage (512b) ... ");
    // Wifi part
    EEPROM.get(0, ssid);      // 32
    EEPROM.get(32, password); // 32

    EEPROM.get(64, beep_enable);    // 1
    EEPROM.get(65, analog_enable);  // 1
    EEPROM.get(66, display_enable); // 1
    EEPROM.get(67, ads1115_enable); // 1

    EEPROM.get(68, post_enable);   // 1
    EEPROM.get(69, api_key);       // 32
    EEPROM.get(101, api_url);      // 256
    EEPROM.get(357, http_timeout); // 2

    // Emon part
    EEPROM.get(359, ct_enable);      // 1
    EEPROM.get(360, ct_calibration); // 8 bytes
    EEPROM.get(368, ct_voltage);     // 8 bytes
    EEPROM.get(376, ct_pf);          // 8 bytes

    // SRNE part
    EEPROM.get(384, srne_enable); // 1

    // SNAT part
    EEPROM.get(385, snat_enable); // 1

    Serial.println("Done");
  }
  else
  {
    saveConfig();
  }

  EEPROM.end();
}

void saveConfig()
{
  EEPROM.begin(512);

  Serial.print("Saving Storage (512b) ... ");
  // Wifi part
  EEPROM.put(0, ssid);      // 32
  EEPROM.put(32, password); // 32

  EEPROM.put(64, beep_enable);    // 1
  EEPROM.put(65, analog_enable);  // 1
  EEPROM.put(66, display_enable); // 1
  EEPROM.put(67, ads1115_enable); // 1

  EEPROM.put(68, post_enable);   // 1
  EEPROM.put(69, api_key);       // 32
  EEPROM.put(101, api_url);      // 256
  EEPROM.put(357, http_timeout); // 2

  // Emon part
  EEPROM.put(359, ct_enable);      // 1
  EEPROM.put(360, ct_calibration); // 8 bytes
  EEPROM.put(368, ct_voltage);     // 8 bytes
  EEPROM.put(376, ct_pf);          // 8 bytes

  // SRNE part
  EEPROM.put(384, srne_enable); // 1

  // SNAT part
  EEPROM.put(385, snat_enable); // 1

  char ok[3] = "OK";
  EEPROM.put(386, ok); // 2
  EEPROM.commit();
  Serial.println("Done");

  EEPROM.end();
}

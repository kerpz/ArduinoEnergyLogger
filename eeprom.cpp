#include <Arduino.h>
// eeprom
#include <EEPROM.h>

#include "eeprom.h"

// globals
char ap_ssid[] = "EnergyLogger-AP"; // limit to 32 bytes
char ap_key[] = "12345678";         // limit to 16 bytes
uint8_t sta_enable = 1;
char sta_ssid[] = "KERPZ-AP2"; // limit to 32 bytes
char sta_key[] = "hackmelol";  // limit to 16 bytes

uint8_t beep_enable = 0;
uint8_t analog_enable = 1;
uint8_t display_enable = 0;
uint8_t ads1115_enable = 0;

uint8_t post_enable = 0;
char api_url[] = "https://192.168.2.1:8001/cgi-bin/custom-full.cgi?a=iot"; // limit to 256
char api_key[] = "NIJCG7UI28O9CAYD";                                       // limit to 32
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
  EEPROM.get(419, ok);

  if (String(ok) == String("OK"))
  {
    Serial.print("Loading Storage (512b) ... ");
    EEPROM.get(0, ap_ssid);     // 32
    EEPROM.get(32, ap_key);     // 16
    EEPROM.get(48, sta_enable); // 1
    EEPROM.get(49, sta_ssid);   // 32
    EEPROM.get(81, sta_key);    // 16

    EEPROM.get(97, beep_enable);     // 1
    EEPROM.get(98, analog_enable);   // 1
    EEPROM.get(99, display_enable);  // 1
    EEPROM.get(100, ads1115_enable); // 1

    EEPROM.get(101, post_enable);  // 1
    EEPROM.get(102, api_url);      // 256
    EEPROM.get(358, api_key);      // 32
    EEPROM.get(390, http_timeout); // 2

    // Emon part
    EEPROM.get(392, ct_enable);      // 1
    EEPROM.get(393, ct_calibration); // 8 bytes
    EEPROM.get(401, ct_voltage);     // 8 bytes
    EEPROM.get(409, ct_pf);          // 8 bytes

    // SRNE part
    EEPROM.get(417, srne_enable); // 1

    // SNAT part
    EEPROM.get(418, snat_enable); // 1

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
  EEPROM.put(0, ap_ssid);     // 32
  EEPROM.put(32, ap_key);     // 16
  EEPROM.put(48, sta_enable); // 1
  EEPROM.put(49, sta_ssid);   // 32
  EEPROM.put(81, sta_key);    // 16

  EEPROM.put(97, beep_enable);     // 1
  EEPROM.put(98, analog_enable);   // 1
  EEPROM.put(99, display_enable);  // 1
  EEPROM.put(100, ads1115_enable); // 1

  EEPROM.put(101, post_enable);  // 1
  EEPROM.put(102, api_url);      // 256
  EEPROM.put(358, api_key);      // 32
  EEPROM.put(390, http_timeout); // 2

  // Emon part
  EEPROM.put(392, ct_enable);      // 1
  EEPROM.put(393, ct_calibration); // 8 bytes
  EEPROM.put(401, ct_voltage);     // 8 bytes
  EEPROM.put(409, ct_pf);          // 8 bytes

  // SRNE part
  EEPROM.put(417, srne_enable); // 1

  // SNAT part
  EEPROM.put(418, snat_enable); // 1

  char ok[3] = "OK";
  EEPROM.put(419, ok); // 2
  EEPROM.commit();
  Serial.println("Done");

  EEPROM.end();
}

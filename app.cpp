// wifi
#include <ESP8266WiFi.h>

#include "app.h"

// https://www.tinkercad.com/things/dJn6LkhyR5E-interface-test

uint16_t run_time = 0;

// timing
int timezone = 8;
uint32_t epoch = 0;
uint8_t second = 0;
uint8_t minute = 0;
uint8_t hour = 0;
uint8_t day = 0;
uint8_t month = 0;
uint16_t year = 0;

float ct_energy; // watt per minute
float pv_energy; // watt per minute
float dc_energy; // watt per minute

void appSetup()
{
  ntpSetup();
  // srne @ D7 D8
  // snat @ D5 D6
  if (display_enable)
    displayLoop();
  // if (dht11_enable)
  //  dht11Loop();

  // if (beep_enable)
  //  beepSetup();
  if (analog_enable)
    analogSetup();
  if (display_enable)
    displaySetup();
  // if (ads1115_enable)
  //  ads1115Setup();
  if (srne_enable)
    srneSetup();
  if (snat_enable)
    snatSetup();
}

void appLoop()
{
  // static float _prev_device_voltage;
  static uint32_t msTick = millis();
  static uint32_t total_ct_power = ct_power;
  static uint32_t total_pv_power = pv_power;
  static uint32_t total_dc_power = dc_power;
  // static uint8_t sTick;

  if (millis() - msTick >= 1000) // 1000ms refresh rate
  {
    msTick = millis();
    ntpLoop();

    if (analog_enable)
      analogLoop();
    delay(1);
    if (srne_enable)
      srneLoop();
    delay(1);
    if (snat_enable)
      snatLoop();
    delay(1);

    if (second >= 59)
    {
      // Convert to watt-minute
      ct_energy = total_ct_power / 60.0;
      pv_energy = total_pv_power / 60.0;
      dc_energy = total_dc_power / 60.0;

      if (post_enable)
      {
        String csv = "&csv=";
        // ct / consumption
        csv += "" + String(ct_voltage, 2); // voltage
        csv += ",0.00";                    // current
        csv += "," + String(ct_energy, 2); // energy W/m
        csv += ",dc";                      // type
        csv += ",0.00";                    // temperature
        csv += ",0.00";                    // charge

        // srne / harvest
        csv += "," + String(pv_voltage, 2); // voltage
        csv += ",0.00";                     // current
        csv += "," + String(pv_energy, 2);  // energy W/m
        csv += ",dc";                       // type
        csv += ",0.00";                     // temperature
        csv += ",0.00";                     // charge

        // srne / battery
        csv += "," + String(battery_voltage, 2);     // voltage
        csv += ",0.00";                              // current
        csv += ",0.00";                              // energy W/m
        csv += ",dc";                                // type
        csv += "," + String(battery_temperature, 2); // temperature
        csv += "," + String(battery_charge, 2);      // charge

        // srne / controller
        csv += ",0.00";                           // voltage
        csv += ",0.00";                           // current
        csv += ",0.00";                           // energy W/m
        csv += ",dc";                             // type
        csv += "," + String(mppt_temperature, 2); // temperature
        csv += ",0.00";                           // charge

        // srne / dc consumption
        csv += "," + String(dc_voltage, 2); // voltage
        csv += ",0.00";                     // current
        csv += "," + String(dc_energy, 2);  // energy W/m
        csv += ",dc";                       // type
        csv += ",0.00";                     // temperature
        csv += ",0.00";                     // charge

        // snat / input
        csv += "," + String(inv_in_voltage, 2);   // input voltage
        csv += ",0.00";                           // input current
        csv += ",0.00";                           // input energy W/m
        csv += ",ac";                             // type
        csv += ",0.00";                           // input temperature
        csv += "," + String(inv_in_frequency, 2); // input frequency

        // snat / output
        csv += "," + String(inv_out_voltage, 2); // output voltage
        csv += ",0.00";                          // output current
        csv += "," + String(inv_out_power, 2);   // output energy W/m
        csv += ",ac";                            // type
        csv += "," + String(inv_temperature, 2); // output temperature
        csv += ",0.00";                          // output frequency

        csv += "," + String(inv_cell_voltage, 2);     // cell voltage
        csv += "," + String(inv_in_fault_voltage, 2); // in fault voltage
        csv += "," + String(inv_flags);               // flags

        postData(csv);
      }

      /*
      if (minute >= 59)
      {
        minute = 0;
        if (hour >= 23)
        {
          hour = 0;
        }
        else
          hour++;
      }
      else
        minute++;
        */

      // resets
      second = 0;
      total_ct_power = ct_power;
      total_pv_power = pv_power;
      total_dc_power = dc_power;
    }
    else
    {
      second++;
      // accumulate power every second
      total_ct_power += ct_power;
      total_pv_power += pv_power;
      total_dc_power += dc_power;
    }
  }
}
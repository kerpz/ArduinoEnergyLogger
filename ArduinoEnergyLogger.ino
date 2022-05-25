/*
 Author:
 - Philip Bordado (kerpz@yahoo.com)

 Hardware:
 - Wemos D1 mini (Compatible board)
 
 Software:
 - Arduino 1.8.12 (Stable)
 - Board 2.7.1, payload bug empty
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <WiFiClientSecureBearSSL.h>
// openssl s_client -connect api.thingspeak.com:443 | openssl x509 -fingerprint -noout
//const uint8_t fingerprint[20] = {0x27, 0x18, 0x92, 0xDD, 0xA4, 0x26, 0xC3, 0x07, 0x09, 0xB9, 0x7A, 0xE6, 0xC5, 0x21, 0xB9, 0x5B, 0x48, 0xF7, 0x16, 0xE1};

#include <SoftwareSerial.h>

//#include <Adafruit_ADS1015.h>
//Adafruit_ADS1115 ads(0x48);

#include <EEPROM.h>

#define APssid "ESSMonitor-AP"
#define APpassword  "12345678"

char ssid[32] = "";
char password[32] = "";

// Bmon part
//float device_voltage;

// Post API
uint8_t post_enable;
char api_key[32] = ""; // NIJCG7UI28O9CAYD
char api_url[256] = ""; // https://api.thingspeak.com/update // https://192.168.2.1:8001/cgi-bin/custom-full.cgi?a=solard
uint16_t http_timeout = 0;

// CT sensor part
uint8_t ct_enable;
float ct_calibration;
float ct_pf;
float ct_current;
float ct_voltage;
float ct_power; // w

// SRNE Modbus part
uint8_t srne_enable;
float battery_voltage;
float battery_charge;
float pv_voltage;
float pv_power;
float mppt_voltage;
float mppt_power;
float battery_temperature;
float mppt_temperature;
float dc_voltage;
float dc_current;
float dc_power;

float min_battery_voltage;
float max_battery_voltage;
float max_charging_current;
float max_discharging_current;
float max_charging_power;
float max_discharging_power;
float battery_charging_amphr;
float battery_discharging_amphr;
float power_generation;
float power_consumption;

uint16_t operating_days;
uint16_t battery_overdischarges;
uint16_t battery_fullcharges;
uint32_t sum_battery_charging_amphr;
uint32_t sum_battery_discharging_amphr;
uint32_t sum_power_generation;
uint32_t sum_power_consumption;
uint8_t load_status;
uint8_t load_brightness;
uint8_t charging_status;

// SNAT Megatec part
uint8_t snat_enable;
float inv_in_voltage;
float inv_in_fault_voltage;
float inv_out_voltage;
float inv_out_power; // w
float inv_in_frequency;
float inv_temperature;
float inv_cell_voltage;
char inv_flags[9] = "";

//
uint8_t beep_enable;
uint8_t display_enable;

uint16_t modbus_error = 0;
uint16_t megatec_error = 0;
uint16_t wifi_error = 0;
uint16_t post_error = 0;
uint16_t http_error = 0;
int http_code = 0;

uint16_t run_time = 0;

// timing
uint8_t second = 0;
//uint8_t minute;
//uint8_t hour;

// Web server
ESP8266WebServer webServer(80);
// DNS server
DNSServer dnsServer;

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

void loadConfig() {
  EEPROM.begin(512);

  // Wifi part
  EEPROM.get(0, ssid); // 32
  EEPROM.get(32, password); // 32

  // Post part
  EEPROM.get(64, post_enable); // 1
  EEPROM.get(65, api_key); // 32
  EEPROM.get(97, api_url); // 256
  EEPROM.get(353, http_timeout); // 2

  // Emon part
  EEPROM.get(355, ct_enable); // 1
  EEPROM.get(356, ct_calibration); // 8 bytes
  EEPROM.get(364, ct_voltage); // 8 bytes
  EEPROM.get(372, ct_pf); // 8 bytes

  // SRNE part
  EEPROM.get(380, srne_enable); // 1
  // SNAT part
  EEPROM.get(381, snat_enable); // 1
  // Display part
  EEPROM.get(382, display_enable); // 1
  // BEEP part
  EEPROM.get(383, beep_enable); // 1

  char ok[3];
  EEPROM.get(384, ok);
  EEPROM.end();
  
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
    post_enable = 0;
    api_key[0] = 0;
    api_url[0] = 0;
    http_timeout = 0;
    ct_calibration = 30.0;
    ct_voltage = 0.0;
    ct_pf = 1.0;
    srne_enable = 0;
    snat_enable = 0;
    display_enable = 0;
    beep_enable = 0;
  }
}

void saveConfig() {
  EEPROM.begin(512);

  // Wifi part
  EEPROM.put(0, ssid); // 32
  EEPROM.put(32, password); // 32

  // Post part
  EEPROM.put(64, post_enable); // 1
  EEPROM.put(65, api_key); // 32
  EEPROM.put(97, api_url); // 256
  EEPROM.put(353, http_timeout); // 2

  // Emon part
  EEPROM.put(355, ct_enable); // 1
  EEPROM.put(356, ct_calibration); // 8 bytes
  EEPROM.put(364, ct_voltage); // 8 bytes
  EEPROM.put(372, ct_pf); // 8 bytes

  // SRNE part
  EEPROM.put(380, srne_enable); // 1
  // SNAT part
  EEPROM.put(381, snat_enable); // 1
  // Dsiplay part
  EEPROM.put(382, display_enable); // 1
  // BEEP part
  EEPROM.put(383, beep_enable); // 1

  char ok[3] = "OK";
  EEPROM.put(384, ok);
  EEPROM.commit();
  EEPROM.end();
}

void execEverySecond() {
  static unsigned long msTick = millis();

  if (millis() - msTick >= 1000) { // run every 1000 ms
    msTick = millis();

    if (ct_enable) extractCT();
    delay(1);
    if (srne_enable) extractSRNE();
    delay(1);
    if (snat_enable) extractSNAT();
    //delay(1);
    if (display_enable) displayData();
    
    if (second >= 59) {
      if (post_enable == 1) postCsv();
      else if (post_enable == 2) postInflux();
      second = 0;
      run_time++;
    }
    else {
      second++;
    }
  }
}

void setup() {
  delay(10);

  Serial.begin(115200);

  Serial.println();
  Serial.println(APssid);

  loadConfig();

  wifiSetup();

  srneSetup();
  snatSetup();

  displaySetup();

  //ads.begin();

  dnsServer.start(53, "*", apIP);

  webserverSetup();

  delay(100);
}

void loop() {
  execEverySecond();

  // dns server
  dnsServer.processNextRequest();
  // web server
  webServer.handleClient();
}

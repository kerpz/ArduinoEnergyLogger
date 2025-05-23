// SRNE Modbus part

// modbus / srne / rs232
#include <SoftwareSerial.h>
SoftwareSerial mbSerial(D7, D8); // RX, TX

uint16_t modbus_error = 0;

float battery_voltage;
float battery_charge;
float mppt_temperature;
float battery_temperature;

float pv_voltage;
// float pv_current;
float pv_power;

float dc_voltage;
// float dc_current;
float dc_power;

/* crc 16 */
uint16_t crc16(uint8_t *buffer, uint8_t length)
{
  uint8_t i, j;
  uint16_t crc = 0xFFFF;
  uint16_t tmp;

  // Calculate the CRC.
  for (i = 0; i < length; i++)
  {
    crc = crc ^ buffer[i];
    for (j = 0; j < 8; j++)
    {
      tmp = crc & 0x0001;
      crc = crc >> 1;
      if (tmp)
      {
        crc = crc ^ 0xA001;
      }
    }
  }
  return crc;
}

void srneSetup()
{
  // modbus srne
  mbSerial.begin(9600);
}

void srneLoop()
{
  uint8_t transmit[8] = {0}; // rtu
  uint8_t data[25] = {0};    // buffer
  int i = 0;

  uint8_t id = 0x01;
  uint8_t command = 0x03;
  uint16_t start_address = 0x0100;
  uint16_t num_registers = 0x000b;

  // build packet for rtu
  transmit[0] = id;      // slave_id;
  transmit[1] = command; // command;
  transmit[2] = start_address >> 8;
  transmit[3] = start_address & 0xFF;
  transmit[4] = num_registers >> 8;
  transmit[5] = num_registers & 0xFF;
  uint16_t crc = crc16(transmit, 6);
  transmit[6] = crc;
  transmit[7] = crc >> 8;

  uint32_t len = (num_registers * 2) + 5;

  for (i = 0; i < 8; i++)
    mbSerial.write(transmit[i]);

  unsigned long timeOut = millis() + 200; // timeout @ 200 ms
  i = 0;
  mbSerial.listen();
  while (i < len && millis() < timeOut)
  {
    if (mbSerial.available())
    {
      data[i] = mbSerial.read();
      i++;
    }
    delay(1); // this is required
  }

  if (i == len)
  {
    // if (crc == crc16(data, 6)) {
    // data[0] // id
    // data[1] //
    // data[2] //

    // extract data
    battery_charge = word(data[3], data[4]);
    battery_voltage = word(data[5], data[6]) * 0.1;
    mppt_temperature = data[9];
    battery_temperature = data[10];

    dc_voltage = word(data[11], data[12]) * 0.1;
    // dc_current = word(data[13], data[14]) * 0.01;
    dc_power = word(data[15], data[16]);

    pv_voltage = word(data[17], data[18]) * 0.1;
    // pv_current = word(data[19], data[20]) * 0.01;
    pv_power = word(data[21], data[22]); // charging power

    // mppt_power = word(data[21], data[22]);
    // if (mppt_power > 0)
    //   mppt_voltage = mppt_power / (word(data[7], data[8]) * 0.01);
    //  load_switch = word(data[23], data[24]);
  }
  else
  {
    modbus_error++;
  }
}

/*
void extractSRNE_daily()
{
  uint8_t transmit[8] = {0}; // rtu
  uint8_t data[23] = {0};    // buffer
  int i = 0;

  uint8_t id = 0x01;
  uint8_t command = 0x03;
  uint16_t start_address = 0x010b;
  uint16_t num_registers = 0x000a;

  // build packet for rtu
  transmit[0] = id;      // slave_id;
  transmit[1] = command; // command;
  transmit[2] = start_address >> 8;
  transmit[3] = start_address & 0xFF;
  transmit[4] = num_registers >> 8;
  transmit[5] = num_registers & 0xFF;
  uint16_t crc = crc16(transmit, 6);
  transmit[6] = crc;
  transmit[7] = crc >> 8;

  uint32_t len = (num_registers * 2) + 5;

  for (i = 0; i < 8; i++)
    mbSerial.write(transmit[i]);

  unsigned long timeOut = millis() + 200; // timeout @ 200 ms
  i = 0;
  mbSerial.listen();
  while (i < len && millis() < timeOut)
  {
    if (mbSerial.available())
    {
      data[i] = mbSerial.read();
      i++;
    }
    delay(1); // this is required
  }

  if (i == len)
  {
    // if (crc == crc16(data, 6)) {
    // data[0] // id
    // data[1] //
    // data[2] //

    // extract data
    min_battery_voltage = word(data[3], data[4]);
    max_battery_voltage = word(data[5], data[6]);
    max_charging_current = word(data[7], data[8]);
    max_discharging_current = word(data[9], data[10]);
    max_charging_power = word(data[11], data[12]);
    max_discharging_power = word(data[13], data[14]);
    battery_charging_amphr = word(data[15], data[16]);
    battery_discharging_amphr = word(data[17], data[18]);
    power_generation = word(data[19], data[20]);
    power_consumption = word(data[21], data[22]);
  }
  else
  {
    modbus_error++;
  }
}

void extractSRNE_history()
{
  uint8_t transmit[8] = {0}; // rtu
  uint8_t data[23] = {0};    // buffer
  int i = 0;

  uint8_t id = 0x01;
  uint8_t command = 0x03;
  uint16_t start_address = 0x0115;
  uint16_t num_registers = 0x000c;

  // build packet for rtu
  transmit[0] = id;      // slave_id;
  transmit[1] = command; // command;
  transmit[2] = start_address >> 8;
  transmit[3] = start_address & 0xFF;
  transmit[4] = num_registers >> 8;
  transmit[5] = num_registers & 0xFF;
  uint16_t crc = crc16(transmit, 6);
  transmit[6] = crc;
  transmit[7] = crc >> 8;

  uint32_t len = (num_registers * 2) + 5;

  for (i = 0; i < 8; i++)
    mbSerial.write(transmit[i]);

  unsigned long timeOut = millis() + 200; // timeout @ 200 ms
  i = 0;
  mbSerial.listen();
  while (i < len && millis() < timeOut)
  {
    if (mbSerial.available())
    {
      data[i] = mbSerial.read();
      i++;
    }
    delay(1); // this is required
  }

  if (i == len)
  {
    // if (crc == crc16(data, 6)) {
    // data[0] // id
    // data[1] //
    // data[2] //

    // extract data
    operating_days = word(data[3], data[4]);
    battery_overdischarges = word(data[5], data[6]);
    battery_fullcharges = word(data[7], data[8]);
    sum_battery_charging_amphr = ((data[9] << 24) | (data[10] << 16) | (data[11] << 8) | data[12]);     // long(data[9], data[10], data[11], data[12]);
    sum_battery_discharging_amphr = ((data[13] << 24) | (data[14] << 16) | (data[15] << 8) | data[16]); // long(data[13], data[14], data[15], data[16]);
    sum_power_generation = ((data[17] << 24) | (data[18] << 16) | (data[19] << 8) | data[20]);          // long(data[17], data[18], data[19], data[20]);
    sum_power_consumption = ((data[21] << 24) | (data[22] << 16) | (data[23] << 8) | data[24]);         // long(data[21], data[22], data[23], data[24]);
    load_status = data[25] >> 7;
    load_brightness = data[25] & 0x7F;
    charging_status = data[26];
  }
  else
  {
    modbus_error++;
  }
}
*/
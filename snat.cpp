// SNAT Megatec part

// megatec / snat / rs232 with RTS or DTR set to 5v
#include <SoftwareSerial.h>
SoftwareSerial mtSerial(D5, D6); // RX, TX

uint16_t megatec_error = 0;

// SNAT Megatec part
float inv_in_voltage;
float inv_in_fault_voltage;
float inv_out_voltage;
float inv_out_power; // w
float inv_in_frequency;
float inv_temperature;
float inv_cell_voltage;
char inv_flags[9] = "";

void snatSetup()
{
  // megatec / snat
  mtSerial.begin(2400);
}

void snatLoop()
{
  char data[47] = {0}; // buffer
  int i = 0;

  uint32_t len = 47;

  mtSerial.write('Q');
  mtSerial.write('1');
  mtSerial.write('\r');

  unsigned long timeOut = millis() + 225; // timeout @ 225 ms
  i = 0;
  mtSerial.listen();
  while (i < len && millis() < timeOut)
  {
    if (mtSerial.available())
    {
      data[i] = mtSerial.read();
      if (!(data[i] >= 48 && data[i] <= 57) && data[i] != 13 && data[i] != 40 && data[i] != 32 && data[i] != 46)
      {
        break;
      }
      i++;
    }
    delay(1); // this is required
  }

  if (i == len &&
      data[0] == '(' &&
      data[6] == ' ' &&
      data[12] == ' ' &&
      data[18] == ' ' &&
      data[22] == ' ' &&
      data[27] == ' ' &&
      data[32] == ' ' &&
      data[37] == ' ' &&
      data[46] == '\r')
  {
    // extract data
    char *token = 0;
    token = strtok(&data[1], " "); // input voltage
    inv_in_voltage = atof(token);
    token = strtok(NULL, " "); // input fault voltage
    inv_in_fault_voltage = atof(token);
    token = strtok(NULL, " "); // output voltage
    inv_out_voltage = atof(token);
    token = strtok(NULL, " "); // output current
    inv_out_power = atof(token) * 10.0;
    token = strtok(NULL, " "); // input frequency
    inv_in_frequency = atof(token);
    token = strtok(NULL, " "); // cell voltage
    inv_cell_voltage = atof(token);
    token = strtok(NULL, " "); // inverter temperature
    inv_temperature = atof(token);
    token = strtok(NULL, "\r"); // flags
    strcpy(inv_flags, token);

    // ct_voltage = inv_in_voltage;
  }
  else
  {
    megatec_error++;
  }
}

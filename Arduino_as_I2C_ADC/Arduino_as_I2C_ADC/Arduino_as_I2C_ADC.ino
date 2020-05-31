#include <Wire.h>

uint8_t analog_readings[10];
int analog_pins[] = {A0, A1, A2, A3, A4};
uint8_t number_of_analog_pins = sizeof(analog_pins) / sizeof(analog_pins[0]);

void setup()
{
  Wire.begin(0x18);
  Wire.onRequest(return_analog_reading);

  for (uint8_t i = 0; i < number_of_analog_pins; i++)
  {
    pinMode(analog_pins[i], INPUT);
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
}

void return_analog_reading()
{
  for (uint8_t i = 0; i < number_of_analog_pins; i++)
  {
    int temp = analogRead(analog_pins[i]);
    analog_readings[i * 2] = temp & 0xFF;
    analog_readings[(i * 2) + 1] = (temp >> 8) & 0xFF;
  }

  Wire.write(analog_readings, 10);
}

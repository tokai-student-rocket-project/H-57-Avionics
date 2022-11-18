#include <Arduino.h>
#include <Wire.h>
#include "LPS22HB.h"

LPS22HB::LPS22HB(int address) : DeviceAddress(address)
{
}

void LPS22HB::initialize()
{
  Wire.beginTransmission(DeviceAddress);
  Wire.write(LPS22HB_CONTROL_REGISTER);
  Wire.write(LPS22HB_CONTROL_75HZ);
  Wire.endTransmission();
}

float LPS22HB::getPressure()
{
  Wire.beginTransmission(DeviceAddress);
  Wire.write(LPS22HB_PRESSURE_OUT);
  Wire.endTransmission();

  Wire.requestFrom(DeviceAddress, 3);
  while (Wire.available() < 3)
    ;
  uint32_t data = Wire.read() | (uint16_t)Wire.read() << 8 | (uint32_t)Wire.read() << 16;
  return data / 4096.0; // 4096で割ると[hPa]になる（データシートp16参照）
}

float LPS22HB::getTemperature()
{
  Wire.beginTransmission(DeviceAddress);
  Wire.write(LPS22HB_TEMPERATURE_OUT);
  Wire.endTransmission();

  Wire.requestFrom(DeviceAddress, 2);
  while (Wire.available() < 2)
    ;
  uint32_t data = Wire.read() | (uint16_t)Wire.read() << 8;
  return data;
}
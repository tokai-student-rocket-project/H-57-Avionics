#include <Arduino.h>
#include "PressureToAltitudeConverter.h"

PressureToAltitudeConverter::PressureToAltitudeConverter()
{
}

void PressureToAltitudeConverter::setConfig(float basePressure, float baseTemperature)
{
  BasePressure = basePressure;
  BaseTemperature = baseTemperature;
}

float PressureToAltitudeConverter::getAltitude(float pressure)
{
  // 以下の式を使用する
  // https://keisan.casio.jp/exec/system/1203469826
  return ((pow((BasePressure / pressure), (1 / 5.25607)) - 1) * (BaseTemperature + 273.15)) / 0.0065;
}
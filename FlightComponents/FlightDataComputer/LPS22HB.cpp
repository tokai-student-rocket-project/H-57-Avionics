#include <Arduino.h>
#include <Wire.h>
#include "LPS22HB.h"

/// @brief コンストラクタ
/// @param deviceAddress AD0がGNDに接続: LPS22HB_ADDRESS_AD0_LOW, AD0がVDDIOに接続: LPS22HB_ADDRESS_AD0_HIGH
LPS22HB::LPS22HB(int deviceAddress) : _deviceAddress(deviceAddress) {
}

/// @brief 75Hzで駆動するよ
void LPS22HB::initialize() {
  Wire.beginTransmission(_deviceAddress);
  Wire.write(LPS22HB_CONTROL_REGISTER);
  Wire.write(LPS22HB_CONTROL_75HZ);
  Wire.endTransmission();
}

/// @brief 気圧を取得する
/// @return 気圧[hPa]
double LPS22HB::getPressure() {
  Wire.beginTransmission(_deviceAddress);
  Wire.write(LPS22HB_PRESSURE_OUT);
  Wire.endTransmission();

  Wire.requestFrom(_deviceAddress, 3);
  while (Wire.available() < 3)
    ;
  uint32_t data = Wire.read() | (uint16_t)Wire.read() << 8 | (uint32_t)Wire.read() << 16;
  return data / 4096.0; // 4096で割ると[hPa]になる（データシートp16参照）
}

/// @brief 基準気圧と基準気温を設定する
/// @param basePressure 基準気圧。ランチャー付近の気圧でいいと思う。
/// @param baseTemperature 基準気温。ランチャー付近の気温でいいと思う。
void LPS22HB::setConfig(double basePressure, double baseTemperature) {
  _basePressure = basePressure;
  _baseTemperature = baseTemperature;
}

/// @brief 高度を取得する
/// @return 高度[m]
double LPS22HB::getAltitude() {
  // 高度を算出する
  // これ見て↓
  // https://keisan.casio.jp/exec/system/1203469826
  // BaseTemperatureはリアルタイムの気温でいいかもしれない
  return ((pow((_basePressure / getPressure()), (1 / 5.25607)) - 1) * (_baseTemperature + 273.15)) / 0.0065;
}
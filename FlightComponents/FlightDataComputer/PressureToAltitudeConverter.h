#ifndef PRESSURE_TO_ALTITUDE_CONVERTER_H
#define PRESSURE_TO_ALTITUDE_CONVERTER_H

class PressureToAltitudeConverter
{
public:
  PressureToAltitudeConverter();

  // 基準値を設定する
  // basePressure: 基準地上気圧
  // baseTemperature: 基準地上温度
  void setConfig(float basePressure, float baseTemperature);

  // 気圧から高度を算出する
  float getAltitude(float pressure);

private:
  // 高度の算出に用いる基準値
  // 初期値に国際標準大気を設定
  float BasePressure = 1013;
  float BaseTemperature = 15;
};

#endif
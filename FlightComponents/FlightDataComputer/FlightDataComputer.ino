#include <Wire.h>
#include <MultiUART.h>
#include <MsgPacketizer.h>
#include "LPS22HB.h"
#include "PressureToAltitudeConverter.h"

// センサのインスタンス
LPS22HB PressureSensor;

// コンバータのインスタンス
PressureToAltitudeConverter PressureToAltitudeConverter;

MultiUART FMC(2, 3);

// フライトデータを構成する数値
float Pressure;
float Altitude;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  FMC.begin(9600);

  PressureSensor.initialize();

  // 初期化直後の外れ値を除くために3秒遅らせる（ローパスフィルタが使えればそっち）
  delay(3000);
  PressureToAltitudeConverter.setConfig(PressureSensor.getPressure(), 24.2);

  // 100HzでPressure, Altitudeを発行する
  MsgPacketizer::publish(FMC, 0x00, Pressure, Altitude)->setFrameRate(100);
}

void loop()
{
  Pressure = PressureSensor.getPressure();
  Altitude = PressureToAltitudeConverter.getAltitude(Pressure);

  MsgPacketizer::post();
}

#include <Wire.h>
#include <MultiUART.h>
#include <Arduino_JSON.h>
#include "LPS22HB.h"
#include "PressureToAltitudeConverter.h"

// センサのインスタンス
LPS22HB PressureSensor;

// コンバータのインスタンス
PressureToAltitudeConverter PressureToAltitudeConverter;

// シリアル通信
// https://github.com/askn37/MultiUART
MultiUART PC(Serial);
MultiUART FMC(2, 3);

// フライトデータをまとめてJSONで出力する
JSONVar FlightData;

// フライトデータを構成する数値
float Pressure;
float Altitude;

void setup()
{
  Wire.begin();
  PC.begin(9600);
  FMC.begin(9600);

  PressureSensor.initialize();

  PressureToAltitudeConverter.setConfig(PressureSensor.getPressure(), 24.2);
}

void loop()
{
  Pressure = PressureSensor.getPressure();
  Altitude = PressureToAltitudeConverter.getAltitude(Pressure);

  // PCへデバッグ情報を送信
  PC.println(Altitude);

  // FlightManegementComputerへフライトデータを送信
  FlightData["pressure"] = Pressure;
  FlightData["altitude"] = Altitude;
  FMC.println(FlightData);

  // 100Hzで送信
  // 同期処理なので厳密に100Hzにはならない
  delay(10);
}

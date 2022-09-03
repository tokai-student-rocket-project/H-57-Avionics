#include <Wire.h>
#include <MultiUART.h>
#include <MsgPacketizer.h>
#include "LPS22HB.h"

namespace sensor {
  // 気圧センサ
  // https://strawberry-linux.com/support/12122/1812122
  const LPS22HB _pressureSensor(LPS22HB_DEFAULT_ADDRESS);
}

namespace serial {
  const MultiUART _flightManagementComputer(2, 3);

  // データ種別
  // これ見て↓
  // https://github.com/tokai-student-rocket-project/Avionics/tree/main/FlightComponents/FlightDataComputer#フライトデータの形式
  constexpr int ALTITUDE_LABEL = 0x00;
}

namespace flightdata {
  double _pressure;
  double _altitude;
}

// フライトデータを構成する数値
double Pressure;
double Altitude;

void setup()
{
  Wire.begin();
  serial::_flightManagementComputer.begin(9600);

  sensor::_pressureSensor.initialize();

  // 初期化直後の外れ値を除くために3秒遅らせる（ローパスフィルタが使えればそっち）
  delay(3000);
  sensor::_pressureSensor.setConfig(sensor::_pressureSensor.getPressure(), 24.2);

  // 100HzでPressure, Altitudeを発行する
  MsgPacketizer::publish(
    serial::_flightManagementComputer,
    serial::ALTITUDE_LABEL,
    flightdata::_altitude)->setFrameRate(100);
}

void loop()
{
  flightdata::_pressure = sensor::_pressureSensor.getPressure();
  flightdata::_altitude = sensor::_pressureSensor.getAltitude();

  MsgPacketizer::post();
}

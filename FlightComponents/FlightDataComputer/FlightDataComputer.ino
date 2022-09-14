#include <Wire.h>
#include <MultiUART.h>
#include <MsgPacketizer.h>
#include "LPS22HB.h"

// データ種別
// これ見て↓
// https://github.com/tokai-student-rocket-project/Avionics/tree/main/FlightComponents/FlightDataComputer#フライトデータの形式
enum class Ident {
  FLIGHT_DATA // 0x00
};

namespace device {
  const MultiUART _flightManagementComputer(2, 3);

  // 気圧センサ
  // https://strawberry-linux.com/support/12122/1812122
  const LPS22HB _pressureSensor(LPS22HB_DEFAULT_ADDRESS);
}

namespace flightdata {
  double _pressure;
  double _altitude;
}

void setup()
{
  Wire.begin();
  device::_flightManagementComputer.begin(9600);

  device::_pressureSensor.initialize();

  // 初期化直後の外れ値を除くために3秒遅らせる（ローパスフィルタが使えればそっち）
  delay(3000);
  device::_pressureSensor.setConfig(device::_pressureSensor.getPressure(), 15);

  // 100Hzでフライトデータを発行する
  MsgPacketizer::publish(
    device::_flightManagementComputer,
    static_cast<int>(Ident::FLIGHT_DATA),
    flightdata::_pressure,
    flightdata::_altitude)->setFrameRate(100);
}

void loop()
{
  flightdata::_pressure = device::_pressureSensor.getPressure();
  flightdata::_altitude = device::_pressureSensor.getAltitude();

  MsgPacketizer::post();
}

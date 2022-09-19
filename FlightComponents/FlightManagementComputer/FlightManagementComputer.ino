#include <Wire.h>
#include "LPS22HB.h"
#include "DescentDetector.h"
#include "FlightPin.h"
#include "Shiranui.h"
#include "LED.h"

enum class FlightMode {
  STANDBY,  // 0x00
  CLIMB,    // 0x01
  DESCENT,  // 0x02
  MAINCHUTE // 0x03
};

namespace device {
  // 気圧センサ
  // https://strawberry-linux.com/support/12122/1812122
  const LPS22HB _pressureSensor(LPS22HB_DEFAULT_ADDRESS);

  const FlightPin _flightPin(12);
  const Shiranui _shiranui3(10);
  const LED _climbIndicator(9);
  const LED _descentIndicator(8);
}

namespace flightData {
  double _pressure;
  double _altitude;
}

namespace internal {
  FlightMode _flightMode;
  unsigned long _launchTime;
}

namespace detector {
  // 引数には平滑化の感度を指定する
  // ブレッドボードを自由落下させるには0.2くらいがちょうどよかった(適当)
  DescentDetector _descentDetector(0.2);
}

namespace separationConfig {
  // 燃焼中に分離しないために保護する時間を指定
  constexpr unsigned long SEPARATION_MINIMUM = 5000;
  // 強制的に分離する時間を指定
  constexpr unsigned long SEPARATION_MAXIMUM = 15000;
  // 分離する高度を指定
  constexpr double SEPARATION_ALTITUDE = -0.5;
}

void setup() {
  Wire.begin();
  Serial.begin(9600);

  device::_pressureSensor.initialize();

  device::_flightPin.initialize();
  device::_shiranui3.initialize();
  device::_climbIndicator.initialize();
  device::_descentIndicator.initialize();

  changeFlightMode(FlightMode::STANDBY);

  // 初期化直後の外れ値を除くために3秒遅らせる（ローパスフィルタが使えればそっち）
  delay(3000);
  device::_pressureSensor.setConfig(device::_pressureSensor.getPressure(), 15);
}

void loop() {
  // センサーを読んでフライトデータを更新する
  flightData::_pressure = device::_pressureSensor.getPressure();
  flightData::_altitude = device::_pressureSensor.getAltitude();
  detector::_descentDetector.updateAltitude(flightData::_altitude);

  // フライトピン刺したらリセット
  if (!device::_flightPin.isReleased()) changeFlightMode(FlightMode::STANDBY);

  // 強制分離
  if (canSeparateForce()) changeFlightMode(FlightMode::MAINCHUTE);

  switch (internal::_flightMode) {
    case FlightMode::STANDBY:
      // フライトピンが抜けたらCLIMBモードに移行
      if (device::_flightPin.isReleased()) changeFlightMode(FlightMode::CLIMB);
      break;
    case FlightMode::CLIMB:
      // 降下が始まったらDESCENTモードに移行
      if (detector::_descentDetector._isDescending) changeFlightMode(FlightMode::DESCENT);
      break;
    case FlightMode::DESCENT:
      // 条件が揃えば分離
      if (canSeparate()) changeFlightMode(FlightMode::MAINCHUTE);
      break;
  }
}

void changeFlightMode(FlightMode newFlightMode) {
  // フライトモードに変更がなければ早期リターン
  if (newFlightMode == internal::_flightMode) return;

  // デバッグ用
  // フライトモードに応じてLEDを切り替える
  device::_climbIndicator.set(newFlightMode == FlightMode::CLIMB);
  device::_descentIndicator.set(newFlightMode == FlightMode::DESCENT);

  switch (newFlightMode)
  {
    case FlightMode::STANDBY:
      device::_shiranui3.reset();
      break;
    case FlightMode::CLIMB:
      // 上昇開始時間を記録しておく
      internal::_launchTime = millis();
      break;
    case FlightMode::MAINCHUTE:
      device::_shiranui3.separate();
      break;
  }

  internal::_flightMode = newFlightMode;
}

bool canSeparate() {
  // 飛行時間がSEPARATION_MINIMUMを超えているか
  bool isPassedSeparationMinimum = millis() > internal::_launchTime + separationConfig::SEPARATION_MINIMUM;
  // 高度がSEPARATION_ALTITUDE以下か
  bool isPassedSeparationAltitude = flightData::_altitude <= separationConfig::SEPARATION_ALTITUDE;

  return isPassedSeparationMinimum && isPassedSeparationAltitude;
}

bool canSeparateForce() {
  // 上昇中か下降中
  bool isInFlight = internal::_flightMode == FlightMode::CLIMB || internal::_flightMode == FlightMode::DESCENT;
  // 飛行時間がSEPARATION_MAXIMUMを超えているか
  bool isPassedSeparationMaximum = millis() > internal::_launchTime + separationConfig::SEPARATION_MAXIMUM;

  return isInFlight && isPassedSeparationMaximum;
}
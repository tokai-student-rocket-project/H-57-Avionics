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

namespace flightdata {
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

namespace separation {
  // 燃焼中に分離しないために保護する時間を指定
  constexpr unsigned long SEPARATION_MINIMUM = 5000;
  // 強制的に分離する時間を指定
  constexpr unsigned long SEPARATION_MAXIMUM = 30000;
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

  internal::_flightMode = FlightMode::STANDBY;

  // 初期化直後の外れ値を除くために3秒遅らせる（ローパスフィルタが使えればそっち）
  delay(3000);
  device::_pressureSensor.setConfig(device::_pressureSensor.getPressure(), 15);
}

void loop() {
  flightdata::_pressure = device::_pressureSensor.getPressure();
  flightdata::_altitude = device::_pressureSensor.getAltitude();

  detector::_descentDetector.updateAltitude(flightdata::_altitude);

  // フライトピン刺したらリセット
  if (!device::_flightPin.isReleased()) {
    device::_shiranui3.reset();
    device::_climbIndicator.off();
    device::_descentIndicator.off();
    internal::_flightMode = FlightMode::STANDBY;
  }

  // バックアップタイマー
  if (internal::_flightMode == FlightMode::CLIMB || internal::_flightMode == FlightMode::DESCENT) {
      if (millis() > internal::_launchTime + separation::SEPARATION_MAXIMUM){
        device::_shiranui3.separate();
      }
  }

  switch (internal::_flightMode) {
    case FlightMode::STANDBY:
      // フライトピンが抜けたらCLIMBモードに移行
      if (device::_flightPin.isReleased()) {
        device::_climbIndicator.on();
        device::_descentIndicator.off();
        internal::_flightMode = FlightMode::CLIMB;
        internal::_launchTime = millis();
      }
      break;
    case FlightMode::CLIMB:
      // DescentDetectorが降下を検出したらDESCENTモードに移行
      if (detector::_descentDetector._isDescending) {
        device::_climbIndicator.off();
        device::_descentIndicator.on();
        internal::_flightMode = FlightMode::DESCENT;
      }
      break;
    case FlightMode::DESCENT:
      // SEPARATE_ALTITUDE以下になれば分離
      if (flightdata::_altitude <= separation::SEPARATION_ALTITUDE) {
        if (millis() > internal::_launchTime + separation::SEPARATION_MINIMUM) {
          device::_shiranui3.separate();
        }
      }
      break;
  }
}
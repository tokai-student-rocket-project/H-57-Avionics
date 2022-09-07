#include <MultiUART.h>
#include <MsgPacketizer.h>
#include "DescentDetector.h"
#include "FlightPin.h"
#include "Shiranui.h"
#include "LED.h"

enum FlightMode {
  STANDBY,
  CLIMB,
  DESCENT,
  MAINCHUTE
};

namespace pin {
  const FlightPin _flightPin(12);
  const Shiranui _shiranui3(10);
  const LED _climbIndicator(9);
  const LED _descentIndicator(8);
}

namespace serial {
  const MultiUART _debug(Serial);
  const MultiUART _flightDataComputer(2, 3);

  // データ種別
  // これ見て↓
  // https://github.com/tokai-student-rocket-project/Avionics/tree/main/FlightComponents/FlightDataComputer#フライトデータの形式
  constexpr int ALTITUDE_LABEL = 0x00;
}

namespace flightdata {
  FlightMode _flightMode;
  double _altitude;
}

namespace detector {
  // 引数には平滑化の感度を指定する
  // ブレッドボードを自由落下させるには0.2くらいがちょうどよかった(適当)
  DescentDetector _descentDetector(0.2);
}

namespace separation {
  // 強制的に分離する時間を指定[s]
  constexpr unsigned long SEPARATE_TIME = 30000;
  constexpr double SEPARATE_ALTITUDE = -0.5;

  unsigned long _launchTime;
}

void setup() {
  // ボーレートを9600以上にするとうまく動かないらしい
  // これ見て↓
  // https://github.com/askn37/MultiUART#指定可能なボーレート
  serial::_debug.begin(9600);
  serial::_flightDataComputer.begin(9600);

  pin::_flightPin.initialize();
  pin::_shiranui3.initialize();
  pin::_climbIndicator.initialize();
  pin::_descentIndicator.initialize();
  flightdata::_flightMode = FlightMode::STANDBY;

  // シリアル通信で高度を購読する
  // これ見て↓
  // https://github.com/hideakitai/MsgPacketizer#direct-data-receive--data-publishing
  MsgPacketizer::subscribe(
    serial::_flightDataComputer,
    serial::ALTITUDE_LABEL,
    [](float altitude) {
      flightdata::_altitude = altitude;
      detector::_descentDetector.updateAltitude(altitude);
    });
}

void loop() {
  MsgPacketizer::parse();

  // フライトピン刺したらリセット
  if (!pin::_flightPin.isReleased()) {
    pin::_shiranui3.reset();
    pin::_climbIndicator.off();
    pin::_descentIndicator.off();
    flightdata::_flightMode = FlightMode::STANDBY;
  }

  // バックアップタイマー
  if (flightdata::_flightMode == FlightMode::CLIMB || flightdata::_flightMode == FlightMode::DESCENT) {
      if (millis() > separation::_launchTime + separation::SEPARATE_TIME){
        pin::_shiranui3.separate();
      }
  }

  switch (flightdata::_flightMode) {
    case FlightMode::STANDBY:
      // フライトピンが抜けたらCLIMBモードに移行 
      if (pin::_flightPin.isReleased()) {
        pin::_climbIndicator.on();
        pin::_descentIndicator.off();
        flightdata::_flightMode = FlightMode::CLIMB;
        separation::_launchTime = millis();
      }
      break;
    case FlightMode::CLIMB:
      // DescentDetectorが降下を検出したらDESCENTモードに移行
      if (detector::_descentDetector._isDescending) {
        pin::_climbIndicator.off();
        pin::_descentIndicator.on();
        flightdata::_flightMode = FlightMode::DESCENT;
      }
      break;
    case FlightMode::DESCENT:
      // SEPARATE_ALTITUDE以下になれば分離
      if (flightdata::_altitude <= separation::SEPARATE_ALTITUDE) {
        pin::_shiranui3.separate();
      }
      break;
  }
}
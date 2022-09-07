#include <MultiUART.h>
#include <MsgPacketizer.h>
#include "DescentDetector.h"
#include "FlightPin.h"

enum FlightMode {
  STANDBY,
  CLIMB,
  DESCENT,
  MAINCHUTE
};

namespace pin {
  const FlightPin _flightPin(12);
  constexpr int SHIRANUI_PIN = 10;
  constexpr int CLIMB_INDICATE_PIN = 9;
  constexpr int DESCENT_INDICATE_PIN = 8;
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

  pinMode(pin::SHIRANUI_PIN, OUTPUT);
  pinMode(pin::CLIMB_INDICATE_PIN, OUTPUT);
  pinMode(pin::DESCENT_INDICATE_PIN, OUTPUT);

  pin::_flightPin.initialize();
  reset();

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
    reset();
  }

  // バックアップタイマー
  if (flightdata::_flightMode == FlightMode::CLIMB || flightdata::_flightMode == FlightMode::DESCENT) {
      if (millis() > separation::_launchTime + separation::SEPARATE_TIME){
        separate();
      }
  }

  switch (flightdata::_flightMode) {
    case FlightMode::STANDBY:
      // フライトピンが抜けたらCLIMBモードに移行 
      if (pin::_flightPin.isReleased()) {
        changeFlightMode(FlightMode::CLIMB);
        separation::_launchTime = millis();
      }
      break;
    case FlightMode::CLIMB:
      // DescentDetectorが降下を検出したらDESCENTモードに移行
      if (detector::_descentDetector._isDescending) {
        changeFlightMode(FlightMode::DESCENT);
      }
      break;
    case FlightMode::DESCENT:
      // SEPARATE_ALTITUDE以下になれば分離
      if (flightdata::_altitude <= separation::SEPARATE_ALTITUDE) {
        separate();
      }
      break;
  }
}

void changeFlightMode(FlightMode nextFlightMode) {

  // 変更がなければ何もせずに終わる
  if (nextFlightMode == flightdata::_flightMode) {
    return;
  }

  // デバッグ用
  // 後で消す
  switch (nextFlightMode)
  {
    case FlightMode::STANDBY:
      digitalWrite(pin::CLIMB_INDICATE_PIN, LOW);
      digitalWrite(pin::DESCENT_INDICATE_PIN, LOW);
      break;
    case FlightMode::CLIMB:
      digitalWrite(pin::CLIMB_INDICATE_PIN, HIGH);
      digitalWrite(pin::DESCENT_INDICATE_PIN, LOW);
      break;
    case FlightMode::DESCENT:
      digitalWrite(pin::CLIMB_INDICATE_PIN, LOW);
      digitalWrite(pin::DESCENT_INDICATE_PIN, HIGH);
      break;
  }

  flightdata::_flightMode = nextFlightMode;
}

void separate() {
  digitalWrite(pin::SHIRANUI_PIN, HIGH);
}

void reset() {
  changeFlightMode(FlightMode::STANDBY);
  digitalWrite(pin::SHIRANUI_PIN, LOW);
}
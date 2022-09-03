#include <MultiUART.h>
#include <MsgPacketizer.h>
#include "DescentDetector.h"

enum FlightMode {
  STANDBY,
  CLIMB,
  DESCENT
};

namespace pin {
  constexpr int FLIGHT_PIN = 12;
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

void setup() {
  // ボーレートを9600以上にするとうまく動かないらしい
  // これ見て↓
  // https://github.com/askn37/MultiUART#指定可能なボーレート
  serial::_debug.begin(9600);
  serial::_flightDataComputer.begin(9600);

  pinMode(pin::FLIGHT_PIN, INPUT_PULLUP);
  pinMode(pin::CLIMB_INDICATE_PIN, OUTPUT);
  pinMode(pin::DESCENT_INDICATE_PIN, OUTPUT);

  changeFlightMode(FlightMode::STANDBY);

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

  switch (flightdata::_flightMode) {
    case FlightMode::STANDBY:
      // フライトピンが抜けたらCLIMBモードに移行 
      if (digitalRead(12) == HIGH) {
        changeFlightMode(FlightMode::CLIMB);
      }
      break;
    case FlightMode::CLIMB:
      // DescentDetectorが降下を検出したらDESCENTモードに移行
      if (detector::_descentDetector.is_descending_) {
        changeFlightMode(FlightMode::DESCENT);
      }
      // フライトピン刺したらリセット
      if (digitalRead(12) == LOW) {
        changeFlightMode(FlightMode::STANDBY);
      }
      break;
    case FlightMode::DESCENT:
      // フライトピン刺したらリセット
      if (digitalRead(12) == LOW) {
        changeFlightMode(FlightMode::STANDBY);
      }
      break;
  }
}

void changeFlightMode(FlightMode nextFlightMode) {
  // モードに応じてLEDを光らせる
  // デバッグ用
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
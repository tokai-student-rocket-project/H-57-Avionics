#include <MultiUART.h>
#include <MsgPacketizer.h>
#include "DescentDetector.h"

namespace pin {
  constexpr int FLIGHT_PIN = 12;
  constexpr int CLIMB_INDICATE_PIN = 9;
  constexpr int DESCENT_INDICATE_PIN = 8;
}

enum FlightMode {
  standby,
  climb,
  descent
};

namespace serial {
  const MultiUART debug(Serial);
  const MultiUART flight_data_computer(2, 3);

  // データ種別、FlightDataComputerのREADME参照
  constexpr int ALTITUDE_LABEL = 0x00;
}

namespace flightdata {
  FlightMode flight_mode;
  double altitude;
}

namespace detector {
  DescentDetector descent_detector(0.2);
}

void setup() {
  serial::debug.begin(9600);
  serial::flight_data_computer.begin(9600);

  pinMode(pin::FLIGHT_PIN, INPUT_PULLUP);
  pinMode(pin::CLIMB_INDICATE_PIN, OUTPUT);
  pinMode(pin::DESCENT_INDICATE_PIN, OUTPUT);

  changeFlightMode(FlightMode::standby);

  MsgPacketizer::subscribe(
    serial::flight_data_computer,
    serial::ALTITUDE_LABEL,
    [](float altitude) {
      flightdata::altitude = altitude;
      detector::descent_detector.update(altitude);
    });
}

void loop() {
  MsgPacketizer::parse();

  switch (flightdata::flight_mode) {
    case FlightMode::standby:
      if (digitalRead(12) == HIGH) {
        changeFlightMode(FlightMode::climb);
      }
      break;
    case FlightMode::climb:
      if (detector::descent_detector.is_descending_) {
        changeFlightMode(FlightMode::descent);
      }
      if (digitalRead(12) == LOW) {
        changeFlightMode(FlightMode::standby);
      }
      break;
    case FlightMode::descent:
      if (digitalRead(12) == LOW) {
        changeFlightMode(FlightMode::standby);
      }
      break;
  }
}

void changeFlightMode(FlightMode nextFlightMode) {
  switch (nextFlightMode)
  {
    case FlightMode::standby:
      digitalWrite(pin::CLIMB_INDICATE_PIN, LOW);
      digitalWrite(pin::DESCENT_INDICATE_PIN, LOW);
      break;
    case FlightMode::climb:
      digitalWrite(pin::CLIMB_INDICATE_PIN, HIGH);
      digitalWrite(pin::DESCENT_INDICATE_PIN, LOW);
      break;
    case FlightMode::descent:
      digitalWrite(pin::CLIMB_INDICATE_PIN, LOW);
      digitalWrite(pin::DESCENT_INDICATE_PIN, HIGH);
      break;
  }

  flightdata::flight_mode = nextFlightMode;
}
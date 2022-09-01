#include <MultiUART.h>
#include <MsgPacketizer.h>
#include "DescentDetector.h"

enum FlightMode {
  standby,
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
  DescentDetector descent_detector;
}

void setup() {
  serial::debug.begin(9600);
  serial::flight_data_computer.begin(9600);

  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);

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
      if (detector::descent_detector.is_descending_) {
        changeFlightMode(FlightMode::descent);
      }
      break;
    case FlightMode::descent:
      break;
  }
}

void changeFlightMode(FlightMode nextFlightMode) {
  if (nextFlightMode == FlightMode::standby) {
    digitalWrite(9, LOW);
    digitalWrite(8, HIGH);
  }
  else {
    digitalWrite(9, HIGH);
    digitalWrite(8, LOW);
  }

  flightdata::flight_mode = nextFlightMode;
}
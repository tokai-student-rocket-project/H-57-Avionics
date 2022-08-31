#include <MultiUART.h>
#include <MsgPacketizer.h>
#include "MovingAverage.h"

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
  FlightMode flight_mode = FlightMode::standby;
  float altitude;
}

namespace descentdetection {
  const MovingAverage pressure_average(0.3);
  float altitude_average_old;
  int descent_count;
}

void setup() {
  serial::debug.begin(9600);
  serial::flight_data_computer.begin(9600);

  pinMode(8, OUTPUT);

  MsgPacketizer::subscribe(
    serial::flight_data_computer,
    serial::ALTITUDE_LABEL,
    [](float altitude) {
      updateAltitude(altitude);
    });
}

void loop() {
  MsgPacketizer::parse();
}

void updateAltitude(float altitude) {
  flightdata::altitude = altitude;

  if (flightdata::flight_mode == FlightMode::standby && isDescending()) {
    flightdata::flight_mode = FlightMode::descent;
    digitalWrite(8, HIGH);
  }
}

bool isDescending() {
  float average = descentdetection::pressure_average.getAverage(flightdata::altitude);

  // 高度が連続で減少した回数をdescent_countに加算
  if (average < descentdetection::altitude_average_old) {
    ++descentdetection::descent_count;
  }
  else {
    descentdetection::descent_count = 0;
  }

  descentdetection::altitude_average_old = average;

  serial::debug.print(flightdata::altitude);
  serial::debug.print("\t");
  serial::debug.print(average);
  serial::debug.print("\t");
  serial::debug.println(descentdetection::descent_count);

  // 10回以上連続で減少したら降下中と判断する
  return descentdetection::descent_count >= 10;
}
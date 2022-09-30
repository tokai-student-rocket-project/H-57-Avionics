#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <SparkFunBME280.h>
#include "PressureSensor.h"
#include "DescentDetector.h"

enum class FlightMode {
  STANDBY,
  FLIGHT,
  PARACHUTE
};


namespace device {
  PressureSensor _pressureSensor;

  constexpr int FLIGHT_PIN = 0;
  constexpr int PROTECTION_INDICATOR = 1;
  constexpr int FLIGHT_INDICATOR = 2;
  constexpr int SEPARATION_INDICATOR = 3;
}

namespace separationConfig {
  constexpr unsigned long SEPARATION_MINIMUM_TIME = 5000;
  constexpr unsigned long SEPARATION_MAXIMUM_TIME = 10000;
}

namespace internal {
  FlightMode _flightMode;
  unsigned long _launchTime;

  DescentDetector _descentDetector(0.2);
}

namespace flightData {
  float _altitude;
}


void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial1.begin(9600);
  LoRa.begin(923E6);

  pinMode(device::FLIGHT_PIN, INPUT_PULLUP);
  pinMode(device::PROTECTION_INDICATOR, OUTPUT);
  pinMode(device::FLIGHT_INDICATOR, OUTPUT);
  pinMode(device::SEPARATION_INDICATOR, OUTPUT);

  device::_pressureSensor.initialize();
  device::_pressureSensor.setReferencePressure(device::_pressureSensor.getPressure());
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    byte command = LoRa.read();

    LoRa.beginPacket();
    LoRa.print("[0x");
    LoRa.print(command, HEX);
    LoRa.print("] ");
    switch (command) {
      case 0x00:
        LoRa.print(device::_pressureSensor.getReferencePressure() / 100.0);
        LoRa.println(" hPa");
        break;
      case 0x01:
        device::_pressureSensor.setReferencePressure(device::_pressureSensor.getPressure());
        LoRa.println("Success.");
        break;
      default:
        LoRa.println("Failure receiving command. Ignored this operation.");
        break;
    }
    LoRa.endPacket();
  }

  updateFlightData();
  updateLED();

  if (internal::_flightMode != FlightMode::STANDBY) {
    logSdCard();
  }

  if (internal::_flightMode == FlightMode::FLIGHT && canSeparateForce()) separate();

  if (digitalRead(device::FLIGHT_PIN) == LOW) reset();

  switch (internal::_flightMode) {
    case FlightMode::STANDBY:
      if (digitalRead(device::FLIGHT_PIN) == HIGH) onLaunched();
      break;
    case FlightMode::FLIGHT:
      if (internal::_descentDetector._isDescending && canSeparate()) separate();
      break;
    case FlightMode::PARACHUTE:
      break;
  }

  delay(10);
}

void updateFlightData() {
  flightData::_altitude = device::_pressureSensor.getAltitude();
  internal::_descentDetector.updateAltitude(flightData::_altitude);
}

void updateLED() {
  digitalWrite(device::PROTECTION_INDICATOR, internal::_flightMode == FlightMode::FLIGHT && !canSeparate());
  digitalWrite(device::FLIGHT_INDICATOR, internal::_flightMode == FlightMode::FLIGHT);
  digitalWrite(device::SEPARATION_INDICATOR, internal::_flightMode == FlightMode::PARACHUTE);
}

void logSdCard() {
  Serial1.print(millis()); Serial1.print("\t");
  Serial1.print(static_cast<int>(internal::_flightMode)); Serial1.print("\t");
  Serial1.println(flightData::_altitude);
}

bool canSeparate() {
  return millis() > internal::_launchTime + separationConfig::SEPARATION_MINIMUM_TIME;
}

bool canSeparateForce() {
  return millis() > internal::_launchTime + separationConfig::SEPARATION_MAXIMUM_TIME;
}

void onLaunched() {
  internal::_flightMode = FlightMode::FLIGHT;
  internal::_launchTime = millis();
}

void separate() {
  internal::_flightMode = FlightMode::PARACHUTE;
}

void reset() {
  internal::_flightMode = FlightMode::STANDBY;
}
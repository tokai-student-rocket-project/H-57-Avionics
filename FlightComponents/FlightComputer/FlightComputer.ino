#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include "BME280Wrap.h"
#include "DescentDetector.h"
#include "FrequencyTimer.h"


enum class FlightMode {
  STANDBY,
  FLIGHT,
  PARACHUTE
};


namespace device {
  // 気圧, 気温, 湿度センサ
  BME280Wrap _bme280;

  // BBMのピン番号
  constexpr int FLIGHT_PIN = 0;
  constexpr int PROTECTION_INDICATOR = 1;
  constexpr int FLIGHT_INDICATOR = 2;
  constexpr int SEPARATION_INDICATOR = 3;
}

namespace separationConfig {
  // 最小分離時間 [ms]
  constexpr unsigned long SEPARATION_MINIMUM_TIME_MS = 3000;
  // 最大分離時間 [ms]
  constexpr unsigned long SEPARATION_MAXIMUM_TIME_MS = 20000;
}

namespace internal {
  FlightMode _flightMode;
  unsigned long _launchTime_ms;

  DescentDetector _descentDetector(0.35);
  FrequencyTimer _frequencyTimer(100);
}

namespace flightData {
  float _temperature_degT;
  float _pressure_Pa;
  float _altitude_m;
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

  device::_bme280.initialize();
  device::_bme280.setReferencePressure(device::_bme280.getPressure());
}


void loop() {
  receiveCommand();

  updateFlightData();
  updateIndicators();
  updateFlightMode();

  if (isInFlight()) {
    writeLog();

    if (canSeparateForce()) {
      internal::_flightMode = FlightMode::PARACHUTE;
      downlinkLog("Separated by timer.");
    }
  }

  internal::_frequencyTimer.delay();
}


void receiveCommand() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    byte command = LoRa.read();

    char message[64];
    switch (command) {
      case 0x00:
        sprintf(message, "[0x00] %.2f hPa", device::_bme280.getReferencePressure() / 100.0);
        break;
      case 0x01:
        device::_bme280.setReferencePressure(device::_bme280.getPressure());
        sprintf(message, "Success.");
        break;
      default:
        sprintf(message, "Failure receiving command. Ignored this operation.");
        break;
    }
    downlinkLog(message);
  }
}


void updateFlightData() {
  flightData::_temperature_degT = device::_bme280.getTemperature();
  flightData::_pressure_Pa      = device::_bme280.getPressure();
  flightData::_altitude_m       = device::_bme280.getAltitude();

  internal::_descentDetector.updateAltitude(flightData::_altitude_m);

  Serial.println(static_cast<int>(internal::_flightMode));
}


void updateIndicators() {
  digitalWrite(device::PROTECTION_INDICATOR, internal::_flightMode == FlightMode::FLIGHT && !canSeparate());
  digitalWrite(device::FLIGHT_INDICATOR, internal::_flightMode == FlightMode::FLIGHT);
  digitalWrite(device::SEPARATION_INDICATOR, internal::_flightMode == FlightMode::PARACHUTE);
}


bool isInFlight() {
  return internal::_flightMode == FlightMode::FLIGHT
      || internal::_flightMode == FlightMode::PARACHUTE;
}


void writeLog() {
  Serial1.print(millis());
  Serial1.print("\t");
  Serial1.print(static_cast<int>(internal::_flightMode));
  Serial1.print("\t");
  Serial1.print(flightData::_altitude_m);
  Serial1.print("\t");
  Serial1.print(flightData::_pressure_Pa);
  Serial1.print("\t");
  Serial1.println(flightData::_temperature_degT);
}


void downlinkLog(char message[]) {
  LoRa.beginPacket();
  LoRa.println(message);
  LoRa.endPacket();
}


bool canSeparate() {
  return millis() > internal::_launchTime_ms + separationConfig::SEPARATION_MINIMUM_TIME_MS;
}

bool canSeparateForce() {
  return millis() > internal::_launchTime_ms + separationConfig::SEPARATION_MAXIMUM_TIME_MS;
}


void updateFlightMode() {
  if (digitalRead(device::FLIGHT_PIN) == LOW) {
    internal::_flightMode = FlightMode::STANDBY;  
  }

  switch (internal::_flightMode) {
    case FlightMode::STANDBY:
      if (digitalRead(device::FLIGHT_PIN) == HIGH) {
        internal::_flightMode = FlightMode::FLIGHT;
        internal::_launchTime_ms = millis();
        downlinkLog("Launched.");
      };
      break;

    case FlightMode::FLIGHT:
      if (internal::_descentDetector._isDescending && canSeparate()) {
        internal::_flightMode = FlightMode::PARACHUTE;
        downlinkLog("Separated by altitude.");
      }
      break;

    case FlightMode::PARACHUTE:
      break;
  }
}
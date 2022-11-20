#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <TaskManager.h>
#include "BME280Wrap.h"
#include "MPU6050.h"
#include "FlightPin.h"
#include "TwoStateDevice.h"
#include "DescentDetector.h"
#include "FrequencyTimer.h"


StaticJsonDocument<1024> upPacket;
StaticJsonDocument<4096> downPacket;


enum class FlightMode {
  STANDBY,
  CLIMB,
  DESCENT,
  PARACHUTE
};


namespace device {
  // 気圧, 気温, 湿度センサ
  BME280Wrap _bme280;

  // 加速度, 角速度センサ
  MPU6050 _mpu6050;

  FlightPin _flightPin(2);

  TwoStateDevice _commandIndicator(LED_BUILTIN);
  TwoStateDevice _protectionIndicator(3);
  TwoStateDevice _flightIndicator(4);
  TwoStateDevice _separationIndicator(5);

  TwoStateDevice _shiranui3(1);
  TwoStateDevice _buzzer(0);
}

namespace separationConfig {
  // 最小分離時間 [ms]
  unsigned long separation_minimum_time_ms = 4000;

  // 最大分離時間 [ms]
  unsigned long separation_maximum_time_ms = 10000;
}

namespace internal {
  FlightMode _flightMode;
  // 離床した瞬間の時間を保存しておく変数
  unsigned long _launchTime_ms;

  // 引数は高度平滑化の強度。手元の試験では0.35がちょうどよかった。
  DescentDetector _descentDetector(0.35);
  // 引数はloopの周期
  FrequencyTimer _frequencyTimer(100);
}

namespace flightData {
  float _temperature_degT;
  float _pressure_Pa;
  float _altitude_m;
  float _acceleration_x_g;
  float _acceleration_y_g;
  float _acceleration_z_g;
  float _gyro_x_degps;
  float _gyro_y_degps;
  float _gyro_z_degps;
}


void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial1.begin(115200);
  LoRa.begin(923E6);

  device::_bme280.initialize();
  device::_bme280.setReferencePressure(device::_bme280.getPressure());

  device::_mpu6050.initialize();
  // 加速度計測の分解能を指定。 FS16の場合は、出力を2048で割ると[G]になる
  device::_mpu6050.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);

  device::_flightPin.initialize();
  device::_commandIndicator.initialize();
  device::_protectionIndicator.initialize();
  device::_flightIndicator.initialize();
  device::_separationIndicator.initialize();
  device::_shiranui3.initialize();
  device::_buzzer.initialize();

  Tasks.add([]{
    downlinkStatus();
    downlinkFlightData();
  })->startIntervalSec(0.5);

  Tasks.add([]{
    downlinkConfig();
  })->startIntervalSec(1.0);

  downlinkEvent("INITIALIZE");
}


void loop() {
  Tasks.update();

  device::_flightPin.update();
  
  updateFlightData();
  updateIndicators();
  updateFlightMode();

  if (canReset()) {
    reset();
    downlinkEvent("RESET");
  }

  if (canSeparate()) {
    separate();
    downlinkEvent("SEPARATE");
  }

  if (canSeparateForce()) {
    separate();
    downlinkEvent("FORCE-SEPARATE");
  }

  if (isFlying()) {
    // writeLog();
  } else {
    receiveCommand();
  }

  internal::_frequencyTimer.delay();
}


void updateFlightData() {
  flightData::_temperature_degT = device::_bme280.getTemperature();
  flightData::_pressure_Pa      = device::_bme280.getPressure();
  flightData::_altitude_m       = device::_bme280.getAltitude();
  
  short ax, ay, az, gx, gy, gz;
  device::_mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  flightData::_acceleration_x_g = ax / 2048.0;
  flightData::_acceleration_y_g = ay / 2048.0;
  flightData::_acceleration_z_g = az / 2048.0;
  flightData::_gyro_x_degps = gx / 16.4;
  flightData::_gyro_y_degps = gy / 16.4;
  flightData::_gyro_z_degps = gz / 16.4;

  internal::_descentDetector.updateAltitude(flightData::_altitude_m);
}


void updateIndicators() {
  device::_protectionIndicator.setState(isFlying() && millis() < internal::_launchTime_ms + separationConfig::separation_minimum_time_ms);
  device::_flightIndicator.setState(isFlying());
  device::_separationIndicator.setState(internal::_flightMode == FlightMode::PARACHUTE);
}


void writeLog() {
  char message[64];
  sprintf(message, "%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
    millis(),
    static_cast<int>(internal::_flightMode),
    flightData::_altitude_m,
    flightData::_pressure_Pa / 100.0,
    flightData::_temperature_degT,
    flightData::_acceleration_x_g,
    flightData::_acceleration_y_g,
    flightData::_acceleration_z_g,
    flightData::_gyro_x_degps,
    flightData::_gyro_y_degps,
    flightData::_gyro_z_degps);
  Serial1.println(message);
}


void downlinkEvent(String event) {
  device::_commandIndicator.on();

  downPacket.clear();
  downPacket["t"] = "e";
  downPacket["ft"] = String((millis() - internal::_launchTime_ms) / 1000.0, 2);
  downPacket["e"] = event;

  LoRa.beginPacket();
  serializeJson(downPacket, LoRa);
  LoRa.endPacket();

  device::_commandIndicator.off();
}


void downlinkStatus() {
  device::_commandIndicator.on();

  downPacket.clear();
  downPacket["t"] = "s";
  downPacket["m"] = String(static_cast<int>(internal::_flightMode));
  downPacket["f"] = device::_flightPin.isReleased() ? "1" : "0";
  downPacket["s3"] = device::_shiranui3.getState() ? "1" : "0";
  downPacket["b"] = device::_buzzer.getState() ? "1" : "0";

  LoRa.beginPacket();
  serializeJson(downPacket, LoRa);
  LoRa.endPacket();

  device::_commandIndicator.off();
}


void downlinkFlightData() {
  if (!isFlying()) return;

  device::_commandIndicator.on();

  downPacket.clear();
  downPacket["t"] = "f";
  downPacket["ft"] = String((millis() - internal::_launchTime_ms) / 1000.0, 2);
  downPacket["alt"] = String(flightData::_altitude_m, 1);
  downPacket["ax"] = String(flightData::_acceleration_x_g, 2);
  downPacket["ay"] = String(flightData::_acceleration_y_g, 2);
  downPacket["az"] = String(flightData::_acceleration_z_g, 2);

  LoRa.beginPacket();
  serializeJson(downPacket, LoRa);
  LoRa.endPacket();

  device::_commandIndicator.off();
}

void downlinkConfig() {
  if (isFlying()) return;

  device::_commandIndicator.on();

  downPacket.clear();
  downPacket["t"] = "c";
  downPacket["p"] = String(device::_bme280.getReferencePressure() / 100.0, 1);
  downPacket["smax"] = String(separationConfig::separation_maximum_time_ms / 1000.0, 2);
  downPacket["smin"] = String(separationConfig::separation_minimum_time_ms / 1000.0, 2);

  LoRa.beginPacket();
  serializeJson(downPacket, LoRa);
  LoRa.endPacket();

  device::_commandIndicator.off();
}


bool isFlying() {
  return internal::_flightMode != FlightMode::STANDBY;
}


bool canReset() {
  return isFlying()
      && !device::_flightPin.isReleased();
}


bool canSeparate() {
  return internal::_flightMode == FlightMode::DESCENT
      && millis() > internal::_launchTime_ms + separationConfig::separation_minimum_time_ms;
}


bool canSeparateForce() {
  return isFlying()
      && internal::_flightMode != FlightMode::PARACHUTE
      && millis() > internal::_launchTime_ms + separationConfig::separation_maximum_time_ms;
}


void separate() {
  device::_shiranui3.on();
  // delay(1000);
  // device::_shiranui3.off();
  device::_buzzer.on();

  changeFlightMode(FlightMode::PARACHUTE);
}


void reset() {
  device::_shiranui3.off();
  device::_buzzer.off();

  changeFlightMode(FlightMode::STANDBY);
}


void updateFlightMode() {
  switch (internal::_flightMode) {
    case FlightMode::STANDBY:
      if (device::_flightPin.isReleased()) {
        changeFlightMode(FlightMode::CLIMB);
        downlinkEvent("LAUNCH");
      };
      break;

    case FlightMode::CLIMB:
      if (internal::_descentDetector._isDescending) {
        changeFlightMode(FlightMode::DESCENT);
        downlinkEvent("DESCENT");
      }
      break;
  }
}


void changeFlightMode(FlightMode nextMode) {
  if (internal::_flightMode == nextMode) {
    return;
  }

  if (nextMode == FlightMode::CLIMB) {
    internal::_launchTime_ms = millis();
  }

  internal::_flightMode = nextMode;
}


void receiveCommand() {
  if (!LoRa.parsePacket()) {
    return;
  }

  deserializeJson(upPacket, LoRa);

  if (upPacket["type"] == "req") {
    device::_commandIndicator.on();

    if (upPacket["req"] == "setRefPress") {
      device::_bme280.setReferencePressure(
        upPacket["v"].is<double>() ? ((double)upPacket["v"] * 100.0) : device::_bme280.getPressure());
    } else if (upPacket["req"] == "setSepaMin") {
      separationConfig::separation_minimum_time_ms = 
        (upPacket["v"].is<double>() ? (double)upPacket["v"] : 4.0) * 1000.0;
    } else if (upPacket["req"] == "setSepaMax") {
      separationConfig::separation_maximum_time_ms = 
        (upPacket["v"].is<double>() ? (double)upPacket["v"] : 10.0) * 1000.0;
    }

    device::_commandIndicator.off();
  }
}
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include "BME280Wrap.h"
#include "MPU6050.h"
#include "DescentDetector.h"
#include "FrequencyTimer.h"


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

  // BBMのピン番号
  constexpr int FLIGHT_PIN = 0;
  constexpr int PROTECTION_INDICATOR = 1;
  constexpr int FLIGHT_INDICATOR = 2;
  constexpr int SEPARATION_INDICATOR = 3;
  constexpr int SHIRANUI3 = 4;
  constexpr int BUZZER = 5;
}

namespace separationConfig {
  // 最小分離時間 [ms]
  constexpr unsigned long SEPARATION_MINIMUM_TIME_MS = 4000;
  // 最大分離時間 [ms]
  constexpr unsigned long SEPARATION_MAXIMUM_TIME_MS = 10000;
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

  pinMode(device::FLIGHT_PIN, INPUT_PULLUP);
  pinMode(device::PROTECTION_INDICATOR, OUTPUT);
  pinMode(device::FLIGHT_INDICATOR, OUTPUT);
  pinMode(device::SEPARATION_INDICATOR, OUTPUT);
  pinMode(device::SHIRANUI3, OUTPUT);
  pinMode(device::BUZZER, OUTPUT);

  device::_bme280.initialize();
  device::_bme280.setReferencePressure(device::_bme280.getPressure());

  device::_mpu6050.initialize();
  // 加速度計測の分解能を指定。 FS16の場合は、出力を2048で割ると[G]になる
  device::_mpu6050.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
}


void loop() {
  receiveCommand();

  updateFlightData();
  updateIndicators();
  updateFlightMode();

  if (isInFlight()) {
    writeLog();

    if (canSeparateForce()) {
      separate();
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

      // 初期化
      case 0x01:
        sprintf(message, "Initialized.");
        break;

      // ヘルスチェック
      case 0x02:
        sprintf(message, "All systems ready to launch.");
        break;

      // 基準気圧取得
      case 0x03:
        sprintf(message, "[0x00] %.2f hPa", device::_bme280.getReferencePressure() / 100.0);
        break;

      // 手動分離
      case 0x04:
        separate();
        downlinkLog("Separated by manual.");
        break;

      // 基準気圧設定
      case 0xF3:
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
  digitalWrite(device::PROTECTION_INDICATOR, isInFlight() && millis() < internal::_launchTime_ms + separationConfig::SEPARATION_MINIMUM_TIME_MS);
  digitalWrite(device::FLIGHT_INDICATOR, isInFlight());
}


bool isInFlight() {
  return internal::_flightMode == FlightMode::CLIMB
      || internal::_flightMode == FlightMode::DESCENT
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
  Serial1.print(flightData::_temperature_degT);
  Serial1.print("\t");
  Serial1.print(flightData::_acceleration_x_g);
  Serial1.print("\t");
  Serial1.print(flightData::_acceleration_y_g);
  Serial1.print("\t");
  Serial1.print(flightData::_acceleration_z_g);
  Serial1.print("\t");
  Serial1.print(flightData::_gyro_x_degps);
  Serial1.print("\t");
  Serial1.print(flightData::_gyro_y_degps);
  Serial1.print("\t");
  Serial1.println(flightData::_gyro_z_degps);
}


void downlinkLog(char message[]) {
  LoRa.beginPacket();
  LoRa.println(message);
  LoRa.endPacket();
}


bool canSeparate() {
  return internal::_flightMode != FlightMode::PARACHUTE
      && millis() > internal::_launchTime_ms + separationConfig::SEPARATION_MINIMUM_TIME_MS;
}


bool canSeparateForce() {
  return internal::_flightMode != FlightMode::PARACHUTE
      && millis() > internal::_launchTime_ms + separationConfig::SEPARATION_MAXIMUM_TIME_MS;
}


void separate() {
  digitalWrite(device::SEPARATION_INDICATOR, true);

  digitalWrite(device::SHIRANUI3, true);
  delay(500);
  digitalWrite(device::SHIRANUI3, false);
  delay(100);
  digitalWrite(device::BUZZER, true);

  internal::_flightMode = FlightMode::PARACHUTE;
}


void updateFlightMode() {
  if (digitalRead(device::FLIGHT_PIN) == LOW) {
    digitalWrite(device::SEPARATION_INDICATOR, false);

    digitalWrite(device::SHIRANUI3, false);
    digitalWrite(device::BUZZER, false);

    internal::_flightMode = FlightMode::STANDBY;
  }

  switch (internal::_flightMode) {
    case FlightMode::STANDBY:
      if (digitalRead(device::FLIGHT_PIN) == HIGH) {
        internal::_flightMode = FlightMode::CLIMB;
        internal::_launchTime_ms = millis();
        downlinkLog("Launched.");
      };
      break;

    case FlightMode::CLIMB:
      if (internal::_descentDetector._isDescending) {
        internal::_flightMode = FlightMode::DESCENT;
        downlinkLog("Apposee detected.");
      }
      break;
    
    case FlightMode::DESCENT:
      if (canSeparate()) {
        separate();
        downlinkLog("Separated by peak.");
      }
  }
}
#include <Wire.h>
// #include <MultiUART.h>
#include <SoftwareSerial.h>
#include <MsgPacketizer.h>
#include <SparkFunBME280.h>
// #include <MPU6050_6Axis_MotionApps20.h>
// #include <MadgwickAHRS.h>
#include "DescentDetector.h"
#include "FlightPin.h"
#include "Shiranui.h"
#include "LED.h"

enum class FlightMode {
  STANDBY, // 0x00
  FLIGHT   // 0x01
};

namespace device {
  // 気圧センサ
  const BME280 _bme280;
  // // 6軸センサ
  // const MPU6050 _mpu6050;

  const LED _flightIndicator(10);
  const Shiranui _shiranui3(11);
  const LED _buzzer(12);
  const FlightPin _flightPin(13);

  const SoftwareSerial _commandReceiver(2, 3);
}

namespace flightData {
  unsigned long _lifeTime;
  double _temperature;
  double _pressure;
  double _altitude;
  // double _acceralationX;
  // double _acceralationY;
  // double _acceralationZ;
  // double _gyroX;
  // double _gyroY;
  // double _gyroZ;
  // double _yaw;
  // double _pitch;
  // double _roll;
}

namespace internal {
  FlightMode _flightMode;
  unsigned long _launchTime;
}

namespace processor {
  // 引数には平滑化の感度を指定する
  // ブレッドボードを自由落下させるには0.2くらいがちょうどよかった(適当)
  const DescentDetector _descentDetector(0.2);

  // // 6軸から姿勢角を計算するやつ
  // const MadgwickFilter _madgwickFilter;
}

namespace separationConfig {
  constexpr unsigned long SEPARATION_MINIMUM = 5000;
  constexpr unsigned long SEPARATION_MAXIMUM = 15000;
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  device::_commandReceiver.begin(9600);

  device::_bme280.beginI2C();
  // device::_mpu6050.initialize();
  // processor::_madgwickFilter.begin(100);

  device::_flightIndicator.initialize();
  device::_shiranui3.initialize();
  device::_buzzer.initialize();
  device::_flightPin.initialize();
}

void loop() {
  flightData::_lifeTime = millis();

  // コマンドを受信する
  if (device::_commandReceiver.available() > 0) {
    int command = device::_commandReceiver.read();
    // if (command == 0x01) {
    device::_commandReceiver.println("COMMAND RECEIVED.");
    // }
  }

  // 温度, 気圧を取得して高度を計算する
  flightData::_temperature = device::_bme280.readTempC();
  flightData::_pressure = device::_bme280.readFloatPressure() / 100.0;
  flightData::_altitude = device::_bme280.readFloatAltitudeMeters();
  processor::_descentDetector.updateAltitude(flightData::_altitude);

  // Serial.println(flightData::_altitude);

  // // スケーリング前の6軸を取得する
  // int ax, ay, az, gx, gy, gz;
  // device::_mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // // スケーリングする
  // flightData::_acceralationX = ax / 2048.0;
  // flightData::_acceralationY = ay / 2048.0;
  // flightData::_acceralationZ = az / 2048.0;
  // flightData::_gyroX = gx / 16.4;
  // flightData::_gyroY = gy / 16.4;
  // flightData::_gyroZ = gz / 16.4;

  // // 姿勢角を計算する
  // processor::_madgwickFilter.updateIMU(
  //   flightData::_gyroX,
  //   flightData::_gyroY,
  //   flightData::_gyroZ,
  //   flightData::_acceralationX,
  //   flightData::_acceralationY,
  //   flightData::_acceralationZ);

  // flightData::_yaw = processor::_madgwickFilter.getYaw();
  // flightData::_pitch = processor::_madgwickFilter.getPitch();
  // flightData::_roll = processor::_madgwickFilter.getRoll();

  // Serial.println(flightData::_pitch);
  

  // フライトピンが抜けている間はログをとる
  // if (device::_flightPin.isReleased()) {
  //   device::_openLog.print(millis());
  //   device::_openLog.print("\t");
  //   device::_openLog.println(static_cast<int>(internal::_flightMode));
  // }

  // フライトモードに応じてLEDを切り替える
  device::_flightIndicator.set(internal::_flightMode == FlightMode::FLIGHT);

  // フライトピン刺したらリセット
  if (!device::_flightPin.isReleased()) {
    internal::_flightMode = FlightMode::STANDBY;
    reset();
  }

  // 強制分離
  if (canSeparateForce()) {
    separate();
  }

  switch (internal::_flightMode) {
    case FlightMode::STANDBY:
      // フライトピンが抜けたらFLIGHTモードに移行
      if (device::_flightPin.isReleased()) {
        internal::_flightMode = FlightMode::FLIGHT;
        internal::_launchTime = flightData::_lifeTime;
      }
      break;
    case FlightMode::FLIGHT:
      // 燃焼終了時間を超えたらCLIMBモードに移行
      if (canSeparate()) {
        separate();
      }
      break;
  }
}

bool canSeparate() {
  return (internal::_flightMode == FlightMode::FLIGHT)
    && (flightData::_lifeTime > internal::_launchTime + separationConfig::SEPARATION_MINIMUM)
    && (processor::_descentDetector._isDescending);
}

bool canSeparateForce() {
  return (internal::_flightMode == FlightMode::FLIGHT)
    && (flightData::_lifeTime > internal::_launchTime + separationConfig::SEPARATION_MAXIMUM);
}

void separate() {
  device::_shiranui3.separate();
  device::_buzzer.on();
}

void reset() {
  device::_shiranui3.reset();
  device::_buzzer.off();
}
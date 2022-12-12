#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <TaskManager.h>
#include "BME280Wrap.h"
#include <MPU6050.h>
#include <MadgwickAHRS.h>
#include <movingAvg.h>
#include "Velocity.h"
#include "FlightPin.h"
#include "TwoStateDevice.h"
#include "DescentDetector.h"


StaticJsonDocument<512> upPacket;
StaticJsonDocument<512> downPacket;


enum class FlightMode {
  STANDBY,
  THRUST,
  CLIMB,
  DESCENT,
  PARACHUTE,
  LAND
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

namespace config {
  // 指定分離高度 [m]
  constexpr float DEFAULT_SEPARATION_ALTITUDE_m = 0.0;
  float separation_altitude_m = config::DEFAULT_SEPARATION_ALTITUDE_m;

  // 想定燃焼時間 [ms]
  constexpr uint32_t DEFAULT_BURN_TIME_ms = 2778;
  uint32_t burn_time_ms = config::DEFAULT_BURN_TIME_ms;

  // 分離保護時間 [ms]
  constexpr uint32_t DEFAULT_SEPARATION_PROTECTION_TIME_ms = 10692;
  uint32_t separation_protection_time_ms = config::DEFAULT_SEPARATION_PROTECTION_TIME_ms;

  // 強制分離時間 [ms]
  constexpr uint32_t DEFAULT_FORCE_SEPARATION_TIME_ms = 12692;
  uint32_t force_separation_time_ms = config::DEFAULT_FORCE_SEPARATION_TIME_ms;

  // 想定着地時間 [ms]
  constexpr uint32_t DEFAULT_LANDING_TIME_ms = 30000;
  uint32_t landing_time_ms = DEFAULT_LANDING_TIME_ms;
}

namespace internal {
  FlightMode _flightMode;
  // 離床した瞬間の時間を保存しておく変数
  uint32_t _launchTime_ms;

  // 引数は高度平滑化の強度。手元の試験では0.35がちょうどよかった。
  DescentDetector _descentDetector(0.35);

  // 姿勢角算出用のフィルタ
  Madgwick _madgwickFilter;

  // 速度算出用
  Velocity _velocity;
  movingAvg _acceleration_x_average_g(50);
  movingAvg _acceleration_y_average_g(50);
  movingAvg _acceleration_z_average_g(50);
}

namespace flightData {
  float _temperature_degT;
  float _pressure_Pa;
  float _altitude_m;
  float _acceleration_x_g;
  float _acceleration_y_g;
  float _acceleration_z_g;
  float _yaw;
  float _pitch;
  float _roll;
  float _speed_mps;
}


void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial1.begin(115200);
  LoRa.begin(920E6);

  device::_bme280.initialize();
  device::_bme280.setReferencePressure(device::_bme280.getPressure());

  device::_mpu6050.initialize();
  // +-16[G]。2048[LBS/G]
  device::_mpu6050.setFullScaleAccelRange(MPU6050_IMU::ACCEL_FS::MPU6050_ACCEL_FS_16);
  // +-2000[deg/s]。16.4[LBS/(deg/s)]
  device::_mpu6050.setFullScaleGyroRange(MPU6050_IMU::GYRO_FS::MPU6050_GYRO_FS_2000);
  device::_mpu6050.CalibrateAccel();
  device::_mpu6050.CalibrateGyro();
  // センサ固有のオフセット。mpu6050ライブラリのIMU_ZEROから求める
  device::_mpu6050.setXAccelOffset(-1665);
  device::_mpu6050.setYAccelOffset(-507);
  device::_mpu6050.setZAccelOffset(1331);
  device::_mpu6050.setXGyroOffset(13);
  device::_mpu6050.setYGyroOffset(17);
  device::_mpu6050.setYGyroOffset(40);

  internal::_madgwickFilter.begin(100);

  internal::_velocity.initialize();
  internal::_acceleration_x_average_g.begin();
  internal::_acceleration_y_average_g.begin();
  internal::_acceleration_z_average_g.begin();

  device::_flightPin.initialize();
  device::_commandIndicator.initialize();
  device::_protectionIndicator.initialize();
  device::_flightIndicator.initialize();
  device::_separationIndicator.initialize();
  device::_shiranui3.initialize();
  device::_buzzer.initialize();

  Tasks.add([] {
    device::_flightPin.update();
  updateFlightData();
  updateIndicators();
  updateFlightMode();
  receiveCommand();
  writeLog();

  if (canReset()) {
    reset();
    changeFlightMode(FlightMode::STANDBY);
    downlinkEvent("RESET");
  }

  if (canSeparateForce()) {
    separate();
    changeFlightMode(FlightMode::PARACHUTE);
    downlinkEvent("FORCE-SEPARATED");
  }
    })->startFps(100);

    Tasks.add([] {
      writeStatusToDownPacket();
    writeFlightDataToDownPacket();
    writeConfigToDownPacket();
    sendDownPacket();
      })->startFps(2);

      // separate関数で使うタスク
      Tasks.add("TurnOffShiranui3TurnOnBuzzer", [] {
        device::_shiranui3.off();
      device::_buzzer.on();
        });

      downlinkEvent("INITIALIZED");

      reset();
}


void loop() {
  Tasks.update();
}


/// @brief センサ類から各種データを読み出す
void updateFlightData() {
  flightData::_temperature_degT = device::_bme280.getTemperature();
  flightData::_pressure_Pa = device::_bme280.getPressure();
  flightData::_altitude_m = device::_bme280.getAltitude();

  internal::_descentDetector.updateAltitude(flightData::_altitude_m);

  int16_t ax, ay, az, gx, gy, gz;
  device::_mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  flightData::_acceleration_x_g = ax / 2048.0;
  flightData::_acceleration_y_g = ay / 2048.0;
  flightData::_acceleration_z_g = az / 2048.0;

  internal::_madgwickFilter.updateIMU(
    gx / 16.4,
    gy / 16.4,
    gz / 16.4,
    flightData::_acceleration_x_g,
    flightData::_acceleration_y_g,
    flightData::_acceleration_z_g);

  flightData::_yaw = internal::_madgwickFilter.getYaw();
  flightData::_pitch = internal::_madgwickFilter.getPitch();
  flightData::_roll = internal::_madgwickFilter.getRoll();

  flightData::_speed_mps = internal::_velocity.getSpeed(
    flightData::_acceleration_x_g,
    flightData::_acceleration_y_g,
    flightData::_acceleration_z_g,
    internal::_acceleration_x_average_g.reading(ax) / 2048.0,
    internal::_acceleration_y_average_g.reading(ay) / 2048.0,
    internal::_acceleration_z_average_g.reading(az) / 2048.0);
}


/// @brief 状況によってLEDを切り替える
void updateIndicators() {
  device::_protectionIndicator.setState(isFlying() && millis() < internal::_launchTime_ms + config::separation_protection_time_ms);
  device::_flightIndicator.setState(isFlying());
  device::_separationIndicator.setState(internal::_flightMode == FlightMode::PARACHUTE);
}


// ロガーにフライトデータを書き込む
void writeLog() {
  // パケット構造
  // <飛行時間[s]>,<フライトモード>,
  // <不知火3の状態>,<ブザーの状態>,
  // <高度[m]>,<降下検出数>,
  // <加速度X[G]>,<加速度Y[G]>,<加速度Z[G]>,
  // <ヨー角[deg]>,<ピッチ角[deg]>,<ロール角[deg]>,<速度[m/s]>\n

  if (!isFlying()) return;

  char log[256];
  sprintf(log, "%.2f,%d,%d,%d,%.2f,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
    (millis() - internal::_launchTime_ms) / 1000.0,
    static_cast<uint8_t>(internal::_flightMode),
    device::_shiranui3.getState() ? 1 : 0,
    device::_buzzer.getState() ? 1 : 0,
    flightData::_altitude_m,
    internal::_descentDetector._descentCount,
    flightData::_acceleration_x_g,
    flightData::_acceleration_y_g,
    flightData::_acceleration_z_g,
    flightData::_yaw,
    flightData::_pitch,
    flightData::_roll,
    flightData::_speed_mps);
  Serial1.println(log);
}


/// @brief イベントをダウンリンクで送信する
/// @param event イベント
void downlinkEvent(String event) {
  // パケット構造
  // {
  //   "e":"<イベント>"
  //  }

  downPacket["e"] = event;

  sendDownPacket();
}


/// @brief ステータスをパケットに書き込む
void writeStatusToDownPacket() {
  // パケット構造
  // {
  //   "m": "<フライトモード>",         ... m->Mode
  //   "f": "<フライトピンの状態>",     ... f->FlightPin
  //   "s3": "<不知火Ⅲの状態>",        ... s3->Shiranui3
  //   "b": "<ブザーの状態>"            ... b->Buzzer
  // }

  downPacket["m"] = String(static_cast<uint8_t>(internal::_flightMode));
  downPacket["f"] = device::_flightPin.isReleased() ? "1" : "0";
  downPacket["s3"] = device::_shiranui3.getState() ? "1" : "0";
  downPacket["b"] = device::_buzzer.getState() ? "1" : "0";
}


/// @brief フライトデータをパケットに書き込む
void writeFlightDataToDownPacket() {
  // パケット構造
  // {
  //   "ft": "<飛行時間[s]>",              ... ft->FlightTime
  //   "alt": "<高度[m]>",                 ... alt->Altitude
  //   "ax": "<加速度X[G]>",               ... ax->AccelerationX
  //   "ay": "<加速度Y[G]>",               ... ay->AccelerationY
  //   "az": "<加速度Z[G]>",               ... az->AccelerationZ
  //   "y": "<ヨー角[deg]>",               ... y->yaw
  //   "p" : "<ピッチ角[deg]>",            ... p->pitch
  //   "r" : "<ロール角[deg]>",            ... r->roll
  //   "s" : "<スピード[m/s]>"             ... s->speed
  // }

  if (!isFlying()) return;

  downPacket["ft"] = String((millis() - internal::_launchTime_ms) / 1000.0, 2);
  downPacket["alt"] = String(flightData::_altitude_m, 1);
  downPacket["ax"] = String(flightData::_acceleration_x_g, 2);
  downPacket["ay"] = String(flightData::_acceleration_y_g, 2);
  downPacket["az"] = String(flightData::_acceleration_z_g, 2);
  downPacket["y"] = String(flightData::_yaw, 2);
  downPacket["p"] = String(flightData::_pitch, 2);
  downPacket["r"] = String(flightData::_roll, 2);
  downPacket["s"] = String(flightData::_speed_mps, 2);
}


/// @brief コンフィグをパケットに書き込む
void writeConfigToDownPacket() {
  // パケット構造
  // {
  //   "a": "<指定分離高度[m]>"         ... a->Altitude
  //   "p": "<基準気圧[hPa]>",          ... p->Pressure
  //   "bt": "<想定燃焼時間[s]>",       ... bt->BurnTime
  //   "sp": "<分離保護時間[s]>",       ... sp->SeparationProtectionTime
  //   "fs": "<強制分離時間[s]>",       ... fs->ForceSeparationTime
  //   "l": "<想定着地時間[s]>"         ... l->LandingTime
  // }

  if (isFlying()) return;

  downPacket["a"] = String(config::separation_altitude_m, 1);
  downPacket["p"] = String(device::_bme280.getReferencePressure() / 100.0, 1);
  downPacket["bt"] = String(config::burn_time_ms / 1000.0, 2);
  downPacket["sp"] = String(config::separation_protection_time_ms / 1000.0, 2);
  downPacket["fs"] = String(config::force_separation_time_ms / 1000.0, 2);
  downPacket["l"] = String(config::landing_time_ms / 1000.0, 2);
}


/// @brief downPacketの内容を送信する。
void sendDownPacket() {
  device::_commandIndicator.on();

  LoRa.beginPacket();
  serializeJson(downPacket, LoRa);
  LoRa.endPacket(true);

  device::_commandIndicator.off();

  downPacket.clear();
}


/// @brief 飛行中かを返す。フライトモードで判断
/// @return True: 飛行中, False: 飛行中でない
bool isFlying() {
  return internal::_flightMode != FlightMode::STANDBY
    && internal::_flightMode != FlightMode::LAND;
}


/// @brief 燃焼終了したかを返す。タイマーで判断
/// @return True: 燃焼終了, False: 燃焼終了でない
bool isBurnout() {
  return internal::_flightMode == FlightMode::THRUST
    && millis() > internal::_launchTime_ms + config::burn_time_ms;
}


/// @brief着地したかを返す。タイマーで判断
/// @return True: 着地, False: 着地でない
bool isLanded() {
  return isFlying()
    && internal::_flightMode != FlightMode::LAND
    && millis() > internal::_launchTime_ms + config::landing_time_ms;
}


/// @brief reset()を実行してもよい状況かを返す
/// @return True: 実行可能, False: 実行不可能
bool canReset() {
  return internal::_flightMode != FlightMode::STANDBY
    && !device::_flightPin.isReleased();
}


/// @brief separate()を実行してもよい状況かを返す
/// @return True: 実行可能, False: 実行不可能
bool canSeparate() {
  return internal::_flightMode == FlightMode::DESCENT
    && millis() > internal::_launchTime_ms + config::separation_protection_time_ms
    && flightData::_altitude_m <= config::separation_altitude_m;
}


/// @brief separate()を強制分離として実行してもよい状況かを返す
/// @return True: 実行可能, False: 実行不可能
bool canSeparateForce() {
  return isFlying()
    && internal::_flightMode != FlightMode::PARACHUTE
    && millis() > internal::_launchTime_ms + config::force_separation_time_ms;
}


/// @brief 分離信号を出す
void separate() {
  device::_shiranui3.on();

  if (!Tasks.getTaskByName("TurnOffShiranui3TurnOnBuzzer")->isRunning())
    Tasks.getTaskByName("TurnOffShiranui3TurnOnBuzzer")->startOnceAfterSec(3.0);
}


/// @brief リセットする
void reset() {
  device::_shiranui3.off();
  device::_buzzer.off();
}


/// @brief 状況に応じてフライトモードを進める
void updateFlightMode() {
  switch (internal::_flightMode) {
  case FlightMode::STANDBY:
    if (device::_flightPin.isReleased()) {
      internal::_launchTime_ms = millis();
      changeFlightMode(FlightMode::THRUST);
      downlinkEvent("LAUNCHED");
    };
    break;

  case FlightMode::THRUST:
    if (isBurnout()) {
      changeFlightMode(FlightMode::CLIMB);
      downlinkEvent("BURNOUT");
    }
    break;

  case FlightMode::CLIMB:
    if (internal::_descentDetector._isDescending) {
      changeFlightMode(FlightMode::DESCENT);
      downlinkEvent("DESCENT");
    }
    break;

  case FlightMode::DESCENT:
    if (canSeparate()) {
      separate();
      changeFlightMode(FlightMode::PARACHUTE);
      downlinkEvent("SEPARATED");
    }
    break;

  case FlightMode::PARACHUTE:
    if (isLanded()) {
      changeFlightMode(FlightMode::LAND);
      downlinkEvent("LANDED");
    }
  }
}


/// @brief 新しいフライトモードに変更する
/// @param nextMode 新しいフライトモード
void changeFlightMode(FlightMode nextMode) {
  if (internal::_flightMode == nextMode) return;

  internal::_flightMode = nextMode;

  writeStatusToDownPacket();
  sendDownPacket();
}


/// @brief アップリンクを受信していれば処理をする
void receiveCommand() {
  if (isFlying()) return;
  if (!LoRa.parsePacket()) return;

  device::_commandIndicator.on();

  deserializeJson(upPacket, LoRa);

  if (upPacket["t"] == "c") {
    if (upPacket["l"] == "a") {
      config::separation_altitude_m =
        upPacket["v"].as<double>() ? (double)upPacket["v"] : config::DEFAULT_SEPARATION_ALTITUDE_m;
    }
    else if (upPacket["l"] == "p") {
      device::_bme280.setReferencePressure(
        upPacket["v"].as<double>() ? ((double)upPacket["v"] * 100.0) : device::_bme280.getPressure());
    }
    else if (upPacket["l"] == "b") {
      config::burn_time_ms =
        upPacket["v"].as<double>() ? (double)upPacket["v"] * 1000.0 : config::DEFAULT_BURN_TIME_ms;
    }
    else if (upPacket["l"] == "sp") {
      config::separation_protection_time_ms =
        upPacket["v"].as<double>() ? (double)upPacket["v"] * 1000.0 : config::DEFAULT_SEPARATION_PROTECTION_TIME_ms;
    }
    else if (upPacket["l"] == "fs") {
      config::force_separation_time_ms =
        upPacket["v"].as<double>() ? (double)upPacket["v"] * 1000.0 : config::DEFAULT_FORCE_SEPARATION_TIME_ms;
    }
    else if (upPacket["l"] == "l") {
      config::landing_time_ms =
        upPacket["v"].as<double>() ? (double)upPacket["v"] * 1000.0 : config::DEFAULT_LANDING_TIME_ms;
    }

    upPacket.clear();
  }

  device::_commandIndicator.off();

  writeConfigToDownPacket();
  sendDownPacket();
}
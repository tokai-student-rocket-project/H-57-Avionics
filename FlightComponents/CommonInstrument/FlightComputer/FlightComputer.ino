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


StaticJsonDocument<256> upPacket;
StaticJsonDocument<256> downPacket;


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
  constexpr double DEFAULT_SEPARATION_ALTITUDE_m = 0.0;
  double separation_altitude_m = config::DEFAULT_SEPARATION_ALTITUDE_m;

  // 想定燃焼時間 [ms]
  constexpr unsigned long DEFAULT_BURN_TIME_ms = 2778;
  unsigned long burn_time_ms = config::DEFAULT_BURN_TIME_ms;

  // 分離保護時間 [ms]
  constexpr unsigned long DEFAULT_SEPARATION_PROTECTION_TIME_ms = 10692;
  unsigned long separation_protection_time_ms = config::DEFAULT_SEPARATION_PROTECTION_TIME_ms;

  // 強制分離時間 [ms]
  constexpr unsigned long DEFAULT_FORCE_SEPARATION_TIME_ms = 12692;
  unsigned long force_separation_time_ms = config::DEFAULT_FORCE_SEPARATION_TIME_ms;

  // 想定着地時間 [ms]
  constexpr unsigned long DEFAULT_LANDING_TIME_ms = 30000;
  unsigned long landing_time_ms = DEFAULT_LANDING_TIME_ms;
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
  Serial.begin(115200);
  Serial1.begin(115200);
  LoRa.begin(920E6);

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
    writeLog();
  })->startIntervalSec(0.1);

  Tasks.add([]{
    downlinkFlightData();
  })->startIntervalSec(0.5);

  Tasks.add([]{
    downlinkStatus();
  })->startIntervalSec(2.0);

  Tasks.add([]{
    downlinkConfig();
  })->startIntervalSec(4.0);

  downlinkEvent("INITIALIZED");

  reset();
}


void loop() {
  Tasks.update();

  device::_flightPin.update();
  
  updateFlightData();
  updateIndicators();
  updateFlightMode();
  receiveCommand();

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

  internal::_frequencyTimer.delay();
}


/// @brief センサ類から各種データを読み出す
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


/// @brief 状況によってLEDを切り替える
void updateIndicators() {
  device::_protectionIndicator.setState(isFlying() && millis() < internal::_launchTime_ms + config::separation_protection_time_ms);
  device::_flightIndicator.setState(isFlying());
  device::_separationIndicator.setState(internal::_flightMode == FlightMode::PARACHUTE);
}


// ロガーにフライトデータを書き込む
void writeLog() {
  // パケット構造
  // <飛行時間[s]>,<フライトモード>,<高度[m]>,
  // <加速度X[G]>,<加速度Y[G]>,<加速度Z[G]>,
  // <角速度X[rad/s]>,<角速度Y[rad/s]>,<角速度Z[rad/s]>\n

  if (!isFlying()) return;

  char message[64];
  sprintf(message, "%.2f,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
    (millis() - internal::_launchTime_ms) / 1000.0,
    static_cast<int>(internal::_flightMode),
    flightData::_altitude_m,
    flightData::_acceleration_x_g,
    flightData::_acceleration_y_g,
    flightData::_acceleration_z_g,
    flightData::_gyro_x_degps,
    flightData::_gyro_y_degps,
    flightData::_gyro_z_degps);
  Serial.println(message);
}


/// @brief イベントをダウンリンクで送信する
/// @param event イベント
void downlinkEvent(String event) {
  // パケット構造
  // {
  //   "t": "e",                  ... t->Type, e->Event
  //   "ft": "<飛行時間[s]>",     ... ft->FlightTime
  //   "e":"<イベント>"
  //  }

  downPacket["t"] = "e";
  downPacket["ft"] = String((millis() - internal::_launchTime_ms) / 1000.0, 2);
  downPacket["e"] = event;

  sendDownPacket();
}


/// @brief ステータスをダウンリンクで送信する
void downlinkStatus() {
  // パケット構造
  // {
  //   "t": "s",                        ... t->Type, s->Status
  //   "m": "<フライトモード>",         ... m->Mode
  //   "f": "<フライトピンの状態>",     ... f->FlightPin
  //   "s3": "<不知火Ⅲの状態>",        ... s3->Shiranui3
  //   "b": "<ブザーの状態>"            ... b->Buzzer
  // }

  downPacket["t"] = "s";
  downPacket["m"] = String(static_cast<int>(internal::_flightMode));
  downPacket["f"] = device::_flightPin.isReleased() ? "1" : "0";
  downPacket["s3"] = device::_shiranui3.getState() ? "1" : "0";
  downPacket["b"] = device::_buzzer.getState() ? "1" : "0";

  sendDownPacket();
}


/// @brief フライトデータをダウンリンクで送信する
void downlinkFlightData() {
  // パケット構造
  // {
  //   "t": "f",                  ... t->Type, f->FlightData
  //   "ft": "<飛行時間[s]>",     ... ft->FlightTime
  //   "alt": "<高度[m]>",        ... alt->Altitude
  //   "ax": "<加速度X[G]>",      ... ax->AccelerationX
  //   "ay": "<加速度Y[G]>",      ... ay->AccelerationY
  //   "az": "<加速度Z[G]>"       ... az->AccelerationZ
  
  // }

  if (!isFlying()) return;

  downPacket["t"] = "f";
  downPacket["ft"] = String((millis() - internal::_launchTime_ms) / 1000.0, 2);
  downPacket["alt"] = String(flightData::_altitude_m, 1);
  downPacket["ax"] = String(flightData::_acceleration_x_g, 2);
  downPacket["ay"] = String(flightData::_acceleration_y_g, 2);
  downPacket["az"] = String(flightData::_acceleration_z_g, 2);

  sendDownPacket();
}


/// @brief コンフィグをダウンリンクで送信する
void downlinkConfig() {
  // パケット構造
  // {
  //   "t": "c",                        ... t->Type, c->Config
  //   "a": "<指定分離高度[m]>"         ... a->Altitude
  //   "p": "<基準気圧[hPa]>",          ... p->Pressure
  //   "b": "<想定燃焼時間[s]>",        ... b->BurnTime
  //   "sp": "<分離保護時間[s]>",       ... sp->SeparationProtectionTime
  //   "fs": "<強制分離時間[s]>",       ... fs->ForceSeparationTime
  //   "l": "<想定着地時間[s]>"         ... l->LandingTime
  // }

  if (isFlying()) return;

  downPacket["t"] = "c";
  downPacket["a"] = String(config::separation_altitude_m, 1);
  downPacket["p"] = String(device::_bme280.getReferencePressure() / 100.0, 1);
  downPacket["b"] = String(config::burn_time_ms / 1000.0, 2);
  downPacket["sp"] = String(config::separation_protection_time_ms / 1000.0, 2);
  downPacket["fs"] = String(config::force_separation_time_ms / 1000.0, 2);
  downPacket["l"] = String(config::landing_time_ms / 1000.0, 2);

  sendDownPacket();
}


/// @brief downPacketの内容を送信する。
void sendDownPacket() {
  device::_commandIndicator.on();

  LoRa.beginPacket();
  serializeJson(downPacket, LoRa);
  LoRa.endPacket();

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
  // delay(1000);
  // device::_shiranui3.off();
  device::_buzzer.on();
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
  downlinkStatus();
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
    } else if (upPacket["l"] == "p") {
      device::_bme280.setReferencePressure(
        upPacket["v"].as<double>() ? ((double)upPacket["v"] * 100.0) : device::_bme280.getPressure());
    } else if (upPacket["l"] == "b") {
      config::burn_time_ms = 
        upPacket["v"].as<double>() ? (double)upPacket["v"] * 1000.0 : config::DEFAULT_BURN_TIME_ms;
    } else if (upPacket["l"] == "sp") {
      config::separation_protection_time_ms = 
        upPacket["v"].as<double>() ? (double)upPacket["v"] * 1000.0 : config::DEFAULT_SEPARATION_PROTECTION_TIME_ms;
    } else if (upPacket["l"] == "fs") {
      config::force_separation_time_ms = 
        upPacket["v"].as<double>() ? (double)upPacket["v"] * 1000.0 : config::DEFAULT_FORCE_SEPARATION_TIME_ms;
    } else if (upPacket["l"] == "l") {
      config::landing_time_ms = 
        upPacket["v"].as<double>() ? (double)upPacket["v"] * 1000.0 : config::DEFAULT_LANDING_TIME_ms;
    }

    upPacket.clear();
  }

  device::_commandIndicator.off();

  downlinkConfig();
}
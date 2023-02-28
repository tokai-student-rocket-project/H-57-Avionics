#include <Wire.h>
#include <LoRa.h>
#include <TaskManager.h>
#include <MsgPacketizer.h>
#include "Altimeter.h"
#include "IMU.h"
#include "Logger.h"
#include "Telemeter.h"
#include "PullupPin.h"
#include "OutputPin.h"


enum class FlightMode {
  STANDBY,
  THRUST,
  CLIMB,
  DESCENT,
  PARACHUTE,
  LAND
};


enum class Event {
  INITIALIZE,
  START,
  LAUNCH,
  BURNOUT,
  APOGEE,
  SEPARATE,
  FORCE_SEPARATE,
  LAND,
  RESET,
  CONFIG_UPDATE,
  EMERGENCY_SEPARATE
};


namespace device {
  // 気圧, 気温, 湿度センサ
  Altimeter _altimeter;

  // 加速度, 角速度センサ
  IMU _imu;

  Logger _logger;
  Telemeter _telemeter;

  PullupPin _dataDumpModePin(7);
  PullupPin _noLogModePin(8);
  PullupPin _flightPin(2);

  OutputPin _commandIndicator(6);
  OutputPin _protectionIndicator(3);
  OutputPin _flightIndicator(4);
  OutputPin _separationIndicator(5);

  OutputPin _shiranui3(1);
  OutputPin _buzzer(0);
}

namespace config {
  // 指定分離高度 [m]
  constexpr float DEFAULT_SEPARATION_ALTITUDE_m = 0.0;
  float separation_altitude_m = config::DEFAULT_SEPARATION_ALTITUDE_m;

  // 想定燃焼時間 [ms]
  constexpr uint32_t DEFAULT_BURN_TIME_ms = 2110;
  uint32_t burn_time_ms = config::DEFAULT_BURN_TIME_ms;

  // 分離保護時間 [ms]
  constexpr uint32_t DEFAULT_SEPARATION_PROTECTION_TIME_ms = 7272;
  uint32_t separation_protection_time_ms = config::DEFAULT_SEPARATION_PROTECTION_TIME_ms;

  // 強制分離時間 [ms]
  constexpr uint32_t DEFAULT_FORCE_SEPARATION_TIME_ms = 13384;
  uint32_t force_separation_time_ms = config::DEFAULT_FORCE_SEPARATION_TIME_ms;

  // 想定着地時間 [ms]
  constexpr uint32_t DEFAULT_LANDING_TIME_ms = 44000;
  uint32_t landing_time_ms = DEFAULT_LANDING_TIME_ms;
}

namespace internal {
  FlightMode _flightMode;
  // 離床した瞬間の時間を保存する変数
  uint32_t _launchTime_ms;
  // 強制分離を実行したかを保存する変数
  bool _isForceSeparated = false;
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
  float _yaw;
  float _pitch;
  float _roll;
  float _voltage33;
  float _voltage5;
  float _voltage12;
}


void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial1.begin(115200);

  // 7番ピンが短絡されていたらデータダンプを開始
  if (!device::_dataDumpModePin.isOpenActually()) {
    while (!Serial);
    device::_logger.dumpLog();
  }

  // フライトピンが開放された状態で起動した時は離床判定しない
  if (device::_flightPin.isOpenActually()) {
    while (true);
  }

  // LoRa 40ch
  LoRa.begin(923.8E6);

  device::_telemeter.stackEvent(static_cast<uint8_t>(Event::INITIALIZE), flightTime());

  device::_altimeter.initialize();
  device::_imu.initialize();

  Tasks.add(mainRoutine)->startIntervalMsec(10);
  Tasks.add(downlinkRoutine)->startIntervalMsec(500);
  Tasks.add("TurnOffShiranuiTask", turnOffShiranuiTask);
  Tasks.add(buzzerOnTask)->startIntervalMsec(1000);
  Tasks.add("BuzzerOffTask", buzzerOffTask);

  MsgPacketizer::subscribe(LoRa, 0xF3, [](uint8_t command, float payload) {executeCommand(command, payload);});

  device::_telemeter.stackEvent(static_cast<uint8_t>(Event::START), flightTime());
}


void loop() {
  Tasks.update();
}


void mainRoutine() {
  device::_flightPin.update();
  updateFlightData();
  updateIndicators();
  updateFlightMode();
  writeLog();

  if (LoRa.parsePacket()) {
    MsgPacketizer::parse();
  }

  if (canReset()) {
    device::_telemeter.stackEvent(static_cast<uint8_t>(Event::RESET), flightTime());

    reset();
    device::_logger.initialize();
    changeFlightMode(FlightMode::STANDBY);
  }

  if (canSeparateForce()) {
    device::_telemeter.stackEvent(static_cast<uint8_t>(Event::FORCE_SEPARATE), flightTime());
    separate();
    internal::_isForceSeparated = true;
    changeFlightMode(FlightMode::PARACHUTE);
  }
}


void downlinkRoutine() {
  device::_telemeter.stackStatus(
    static_cast<uint8_t>(internal::_flightMode),
    device::_flightPin.isOpen(),
    device::_shiranui3.isOn(),
    device::_buzzer.isOn(),
    flightData::_voltage33,
    flightData::_voltage5,
    flightData::_voltage12
  );

  if (isFlying()) {
    device::_telemeter.stackFlightData(
      flightTime(),
      flightData::_altitude_m,
      flightData::_acceleration_x_g + flightData::_acceleration_y_g + flightData::_acceleration_z_g,
      flightData::_yaw,
      flightData::_pitch,
      flightData::_roll
    );
  }
  else {
    device::_telemeter.stackConfig(
      config::separation_altitude_m,
      device::_altimeter.getReferencePressure() / 100.0,
      config::burn_time_ms / 1000.0,
      config::separation_protection_time_ms / 1000.0,
      config::force_separation_time_ms / 1000.0,
      config::landing_time_ms / 1000.0
    );
  }

  device::_commandIndicator.on();
  device::_telemeter.sendStackedData();
  device::_commandIndicator.off();
}


void turnOffShiranuiTask() {
  device::_shiranui3.off();
  device::_buzzer.on();
}

void buzzerOnTask() {
  if (!isFlying() || internal::_flightMode == FlightMode::PARACHUTE) return;

  device::_buzzer.on();
  Tasks["BuzzerOffTask"]->startOnceAfterMsec(200);
}

void buzzerOffTask() {
  device::_buzzer.off();
}


/// @brief センサ類から各種データを読み出す
void updateFlightData() {
  flightData::_temperature_degT = device::_altimeter.getTemperature();
  flightData::_pressure_Pa = device::_altimeter.getPressure();
  flightData::_altitude_m = device::_altimeter.getAltitude();

  device::_imu.getData(
    &flightData::_acceleration_x_g,
    &flightData::_acceleration_y_g,
    &flightData::_acceleration_z_g,
    &flightData::_gyro_x_degps,
    &flightData::_gyro_y_degps,
    &flightData::_gyro_z_degps,
    &flightData::_yaw,
    &flightData::_pitch,
    &flightData::_roll);

  // 測定電圧 = ADC出力値 / 精度 * 最大電圧 * 電圧係数
  // 電圧係数 = 基準電圧 / 2.4
  flightData::_voltage33 = analogRead(A6) / 1024.0 * 3.3 * 1.37;
  flightData::_voltage5 = analogRead(A5) / 1024.0 * 3.3 * 2.08;
  flightData::_voltage12 = analogRead(A4) / 1024.0 * 3.3 * 5.00;
}


/// @brief 状況によってLEDを切り替える
void updateIndicators() {
  device::_protectionIndicator.set(isFlying() && millis() < internal::_launchTime_ms + config::separation_protection_time_ms);
  device::_flightIndicator.set(isFlying());
  device::_separationIndicator.set(internal::_flightMode == FlightMode::PARACHUTE);
}


// ロガーにフライトデータを書き込む
void writeLog() {
  if (!isFlying()) return;

  // 8番ピンが短絡されていたら書き込みを行わない
  // EEPROM書き込み回数の温存用
  if (!device::_noLogModePin.isOpenActually()) {
    return;
  }

  device::_logger.writeLog(
    flightTime(),
    static_cast<uint8_t>(internal::_flightMode),
    device::_shiranui3.isOn(),
    device::_buzzer.isOn(),
    flightData::_pressure_Pa,
    flightData::_temperature_degT,
    flightData::_altitude_m,
    device::_altimeter.descentCount(),
    flightData::_acceleration_x_g,
    flightData::_acceleration_y_g,
    flightData::_acceleration_z_g,
    flightData::_gyro_x_degps,
    flightData::_gyro_y_degps,
    flightData::_gyro_z_degps,
    flightData::_yaw,
    flightData::_pitch,
    flightData::_roll,
    flightData::_voltage33,
    flightData::_voltage5,
    flightData::_voltage12
  );
}


/// @brief 飛行時間を返す。飛行中でなければ -1.0 を返す
/// @return 飛行時間 [s]
float flightTime() {
  if (!isFlying()) return -1.0;

  return (millis() - internal::_launchTime_ms) / 1000.0;
}


/// @brief 頂点分離設定かを返す。指定分離高度を0mに設定していれば頂点分離する
/// @return True: 頂点分離, False: 指定高度分離
bool isApogeeSeparation() {
  return config::separation_altitude_m == 0.0;
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
    && !device::_flightPin.isOpen();
}


/// @brief separate()を実行してもよい状況かを返す
/// @return True: 実行可能, False: 実行不可能
bool canSeparate() {
  // 降下中でなければ分離実行不可
  if (internal::_flightMode != FlightMode::DESCENT) return false;
  // 分離保護時間なら分離実行不可
  if (millis() <= internal::_launchTime_ms + config::separation_protection_time_ms) return false;

  // 頂点分離設定では降下中なら常に分離実行可能
  if (isApogeeSeparation()) return true;

  // 指定分離高度を下回れば分離実行可能
  return  flightData::_altitude_m <= config::separation_altitude_m;
}


/// @brief separate()を強制分離として実行してもよい状況かを返す
/// @return True: 実行可能, False: 実行不可能
bool canSeparateForce() {
  return isFlying()
    && !internal::_isForceSeparated
    && millis() > internal::_launchTime_ms + config::force_separation_time_ms;
}


/// @brief 分離信号を出す
void separate() {
  device::_buzzer.off();
  device::_shiranui3.on();

  if (!Tasks["TurnOffShiranuiTask"]->isRunning()) {
    Tasks["TurnOffShiranuiTask"]->startOnceAfterMsec(3000);
  }
}


/// @brief リセットする
void reset() {
  device::_shiranui3.off();
  device::_buzzer.off();
  internal::_isForceSeparated = false;
}


/// @brief 状況に応じてフライトモードを進める
void updateFlightMode() {
  switch (internal::_flightMode) {
  case FlightMode::STANDBY:
    if (device::_flightPin.isOpen()) {
      // フライトピンのチャタリング対策で10回連続を取るため、0.1secのタイムラグが出る。
      internal::_launchTime_ms = millis() - 100;
      changeFlightMode(FlightMode::THRUST);
      device::_telemeter.stackEvent(static_cast<uint8_t>(Event::LAUNCH), flightTime());
    };
    break;

  case FlightMode::THRUST:
    if (isBurnout()) {
      device::_telemeter.stackEvent(static_cast<uint8_t>(Event::BURNOUT), flightTime());
      changeFlightMode(FlightMode::CLIMB);
    }
    break;

  case FlightMode::CLIMB:
    if (device::_altimeter.isDescending()) {
      device::_telemeter.stackEvent(static_cast<uint8_t>(Event::APOGEE), flightTime());
      changeFlightMode(FlightMode::DESCENT);
    }
    break;

  case FlightMode::DESCENT:
    if (canSeparate()) {
      device::_telemeter.stackEvent(static_cast<uint8_t>(Event::SEPARATE), flightTime());
      separate();
      changeFlightMode(FlightMode::PARACHUTE);
    }
    break;

  case FlightMode::PARACHUTE:
    if (isLanded()) {
      device::_telemeter.stackEvent(static_cast<uint8_t>(Event::LAND), flightTime());
      changeFlightMode(FlightMode::LAND);
    }
  }
}


/// @brief 新しいフライトモードに変更する
/// @param nextMode 新しいフライトモード
void changeFlightMode(FlightMode nextMode) {
  if (internal::_flightMode == nextMode) return;

  internal::_flightMode = nextMode;
}


/// @brief コマンドを実行する
/// @param command 0x00:指定分離高度
/// @param command 0x01:基準気圧
/// @param command 0x02:想定燃焼時間
/// @param command 0x03:分離保護時間
/// @param command 0x04:強制分離時間
/// @param command 0x05:想定着地時間
/// @param command 0xFF:緊急分離
/// @param payload 
void executeCommand(uint8_t command, float payload) {
  device::_commandIndicator.on();

  device::_telemeter.stackEvent(static_cast<uint8_t>(Event::CONFIG_UPDATE), flightTime());

  switch (command)
  {
  case 0x00: // 指定分離高度
    config::separation_altitude_m = payload ? payload : config::DEFAULT_SEPARATION_ALTITUDE_m;
    break;
  case 0x01: // 基準気圧
    device::_altimeter.setReferencePressure(payload ? payload * 100.0 : device::_altimeter.getPressure());
    break;
  case 0x02: // 想定燃焼時間
    config::burn_time_ms = payload ? payload * 1000.0 : config::DEFAULT_BURN_TIME_ms;
    break;
  case 0x03: // 分離保護時間
    config::separation_protection_time_ms = payload ? payload * 1000.0 : config::DEFAULT_SEPARATION_PROTECTION_TIME_ms;
    break;
  case 0x04: // 強制分離時間
    config::force_separation_time_ms = payload ? payload * 1000.0 : config::DEFAULT_FORCE_SEPARATION_TIME_ms;
    break;
  case 0x05: // 想定着地時間
    config::landing_time_ms = payload ? payload * 1000.0 : config::DEFAULT_LANDING_TIME_ms;
    break;
  case 0xFF: // 緊急分離
    device::_telemeter.stackEvent(static_cast<uint8_t>(Event::EMERGENCY_SEPARATE), flightTime());
    separate();
    internal::_isForceSeparated = true;
    changeFlightMode(FlightMode::PARACHUTE);
  default:
    break;
  }

  device::_commandIndicator.off();
}
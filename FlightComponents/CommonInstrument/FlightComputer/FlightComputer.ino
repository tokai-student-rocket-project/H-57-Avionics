#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <IntervalCounter.h>
#include <OneShotTimer.h>
#include <MsgPacketizer.h>
#include "BME280Wrap.h"
#include <MPU6050.h>
#include <MadgwickAHRS.h>
#include <movingAvg.h>
#include "Logger.h"
#include "Velocity.h"
#include "FlightPin.h"
#include "TwoStateDevice.h"
#include "DescentDetector.h"
#include "Telemeter.h"


StaticJsonDocument<512> upPacket;


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

  Logger _logger;
  Telemeter _telemeter;
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
  // タイマー
  IntervalCounter _downlinkInterval(0.5);
  IntervalCounter _logicInterval(0.01);
  OneShotTimer _separateShot(3.0);

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
  float _speed_mps;
  float _acceleration_x_g;
  float _acceleration_y_g;
  float _acceleration_z_g;
  float _gyro_x_degps;
  float _gyro_y_degps;
  float _gyro_z_degps;
  float _yaw;
  float _pitch;
  float _roll;
}


void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial1.begin(115200);
  LoRa.begin(920E6);

  downlinkEvent("INITIALIZE");

  device::_bme280.initialize();
  device::_bme280.setReferencePressure(device::_bme280.getPressure());

  device::_mpu6050.initialize();
  // +-16[G]。2048[LBS/G]
  device::_mpu6050.setFullScaleAccelRange(MPU6050_IMU::ACCEL_FS::MPU6050_ACCEL_FS_16);
  // +-2000[deg/s]。16.4[LBS/(deg/s)]
  device::_mpu6050.setFullScaleGyroRange(MPU6050_IMU::GYRO_FS::MPU6050_GYRO_FS_2000);
  // device::_mpu6050.CalibrateAccel();
  // device::_mpu6050.CalibrateGyro();
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

  MsgPacketizer::subscribe(LoRa, 0xF3,
    [](
      uint8_t command,
      float payload
      )
    {
      if (!isFlying()) executeCommand(command, payload);
    }
  );

  // ロジック用タイマー 0.01秒間隔
  internal::_logicInterval.onUpdate([&]() {
    logic();
    });
  internal::_logicInterval.start();

  // ダウンリンク用タイマー1 0.5秒間隔
  internal::_downlinkInterval.onUpdate([&]() {
    downlinkStatus();
  downlinkFlightData();
  downlinkConfig();

  device::_commandIndicator.on();
  device::_telemeter.sendDownlink();
  device::_commandIndicator.off();
    });
  internal::_downlinkInterval.start();

  // 分離3秒後に電磁弁をオフにする
  internal::_separateShot.onUpdate([&]() {
    device::_shiranui3.off();
  device::_buzzer.on();
    });

  reset();

  downlinkEvent("START");
}


void loop() {
  internal::_logicInterval.update();
  internal::_downlinkInterval.update();
  internal::_separateShot.update();
}


void logic() {
  device::_flightPin.update();
  updateFlightData();
  updateIndicators();
  updateFlightMode();
  writeLog();

  if (LoRa.parsePacket()) {
    MsgPacketizer::parse();
  }

  if (canReset()) {
    downlinkEvent("RESET");
    reset();
    device::_logger.initialize();
    changeFlightMode(FlightMode::STANDBY);
  }

  if (canSeparateForce()) {
    downlinkEvent("FORCE-SEPARATE");
    separate();
    changeFlightMode(FlightMode::PARACHUTE);
  }
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
  flightData::_gyro_x_degps = gx / 16.4;
  flightData::_gyro_y_degps = gy / 16.4;
  flightData::_gyro_z_degps = gz / 16.4;

  internal::_madgwickFilter.updateIMU(
    flightData::_gyro_x_degps,
    flightData::_gyro_y_degps,
    flightData::_gyro_z_degps,
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

  device::_logger.writeLog(
    flightTime(),
    static_cast<uint8_t>(internal::_flightMode),
    device::_shiranui3.getState() ? 1 : 0,
    device::_buzzer.getState() ? 1 : 0,
    flightData::_altitude_m,
    flightData::_speed_mps,
    internal::_descentDetector._descentCount,
    flightData::_acceleration_x_g,
    flightData::_acceleration_y_g,
    flightData::_acceleration_z_g,
    flightData::_gyro_x_degps,
    flightData::_gyro_y_degps,
    flightData::_gyro_z_degps,
    flightData::_yaw,
    flightData::_pitch,
    flightData::_roll
  );
}


/// @brief イベントをダウンリンクで送信する
/// @param event イベント
void downlinkEvent(String event) {
  device::_telemeter.sendEvent(
    flightTime(),
    event
  );
}


/// @brief ステータスをダウンリンクで送信する
void downlinkStatus() {
  device::_telemeter.sendStatus(
    static_cast<uint8_t>(internal::_flightMode),
    device::_flightPin.isReleased(),
    device::_shiranui3.getState(),
    device::_buzzer.getState(),
    analogRead(A1) / 1024.0 * 3.3 * 1.37,
    analogRead(A0) / 1024.0 * 3.3 * 2.08,
    analogRead(A2) / 1024.0 * 3.3 * 5.00
  );
}


/// @brief フライトデータをダウンリンクで送信する
void downlinkFlightData() {
  if (!isFlying()) return;

  device::_telemeter.sendFlightData(
    flightTime(),
    flightData::_altitude_m,
    flightData::_speed_mps,
    flightData::_yaw,
    flightData::_pitch,
    flightData::_roll
  );
}


/// @brief コンフィグをダウンリンクで送信する
void downlinkConfig() {
  if (isFlying()) return;

  device::_telemeter.sendConfig(
    config::separation_altitude_m,
    device::_bme280.getReferencePressure() / 100.0,
    config::burn_time_ms / 1000.0,
    config::separation_protection_time_ms / 1000.0,
    config::force_separation_time_ms / 1000.0,
    config::landing_time_ms / 1000.0
  );
}

/// @brief 飛行時間を返す。飛行中でなければ -1.0 を返す
/// @return 飛行時間 [s]
float flightTime() {
  if (!isFlying()) return -1.0;

  return (millis() - internal::_launchTime_ms) / 1000.0;
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

  if (!internal::_separateShot.isRunning())
    internal::_separateShot.start();
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
      downlinkEvent("LAUNCH");
    };
    break;

  case FlightMode::THRUST:
    if (isBurnout()) {
      downlinkEvent("BURNOUT");
      changeFlightMode(FlightMode::CLIMB);
    }
    break;

  case FlightMode::CLIMB:
    if (internal::_descentDetector._isDescending) {
      downlinkEvent("DESCENT");
      changeFlightMode(FlightMode::DESCENT);
    }
    break;

  case FlightMode::DESCENT:
    if (canSeparate()) {
      downlinkEvent("SEPARATE");
      separate();
      changeFlightMode(FlightMode::PARACHUTE);
    }
    break;

  case FlightMode::PARACHUTE:
    if (isLanded()) {
      downlinkEvent("LAND");
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
/// @param command 0x04 強制分離時間
/// @param command 0x05:想定着地時間
/// @param payload 
void executeCommand(uint8_t command, float payload) {
  device::_commandIndicator.on();

  downlinkEvent("CONFIG-UPDATE");

  switch (command)
  {
  case 0x00: // 指定分離高度
    config::separation_altitude_m = payload ? payload : config::DEFAULT_SEPARATION_ALTITUDE_m;
    break;
  case 0x01: // 基準気圧
    device::_bme280.setReferencePressure(payload ? payload * 100.0 : device::_bme280.getPressure());
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
  default:
    break;
  }

  device::_commandIndicator.off();
}
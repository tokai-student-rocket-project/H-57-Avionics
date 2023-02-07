# プログラム詳細

Arduinoに書き込むプログラムは[FlightComputer](../FlightComputer/)以下にあります。メインのファイルは[FlightComputer.ino](../FlightComputer/FlightComputer.ino)です。

FlightComputer.ino以外にも自作ライブラリが複数あるので、これらも併せて説明します。

## [FlightComputer.ino](../FlightComputer/FlightComputer.ino)

### 依存するライブラリ

- Wire([Github](https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/Wire) | [Arduino Reference](https://www.arduino.cc/reference/en/language/functions/communication/wire/))
  - I2C接続用
- LoRa([Github](https://github.com/sandeepmistry/arduino-LoRa))
  - 無線通信用
- TaskManager([Github](https://github.com/hideakitai/TaskManager))
  - 周期動作用
  - 動作レートを100Hz、ダウンリンクを2Hzなど周期的に処理を実行することができます。
- MsgPacketizer([Github](https://github.com/hideakitai/MsgPacketizer))
  - ログパケットやテレメトリパケットのシリアライズ/デシリアライズ用
  - 軽量かつ堅牢なバイナリ配列であるMsgPackを生成できます。
  - パケットについては[Qiitaの記事](https://qiita.com/hideakitai/items/347985528656be03b620#%E3%82%A8%E3%83%B3%E3%82%B3%E3%83%BC%E3%83%89)を参考にしてください。
- Altimeter([解説]())
  - 自作
  - 気圧センサBME280の抽象化用
- IMU([解説]())
  - 自作
  - 6軸センサMPU6050の抽象化用
- Logger([解説]())
  - 自作
  - ログパケットのシリアライズ/デシリアライズ及び、EEPROM/SDカードロガーの扱い簡略化用
- Telemeter([解説]())
  - 自作
  - テレメトリパケットのシリアライズ/デシリアライズ及び、LoRaの扱い簡略化用
- PullupPin([解説]())
  - 自作
  - プルアップ設定ピンの抽象化用
- OutputPin([解説]())
  - 自作
  - アウトプット設定ピンの抽象化用

### 解説

FlightComputer.inoの内容を解説します。C++の仕様や構文は適宜ググってください。

基本は上(0行目)から下の順番に解説します。行番号はプログラムの修正や変更で変わる可能性があります。

#### 0~10行目

```cpp
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
```

依存するライブラリを指定しています。コンパイルすると、#includeの位置に指定したファイルの内容が挿入されます。

#### 13~20行目

```cpp
enum class FlightMode {
  STANDBY,
  THRUST,
  CLIMB,
  DESCENT,
  PARACHUTE,
  LAND
};
```

[フライトモード](../README.md#フライトモード)をここで定義しています。

#### 23~44行目

```cpp
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
```

センサやピンなどを抽象化するライブラリをインスタンス化して変数として保持しています。

`namespace`  
グローバルに変数が乱立することを避けるために、全ての変数を名前空間で細分化しています。例として`_altimeter`にアクセスしたいときは、`device::_altimeter`のように変数を包含する名前空間を指定する必要があります。

ここからは名前空間による変数定義が続きます。

#### 46~66行目

```cpp
namespace config {
  // 指定分離高度 [m]
  constexpr float DEFAULT_SEPARATION_ALTITUDE_m = 0.0;
  float separation_altitude_m = config::DEFAULT_SEPARATION_ALTITUDE_m;

  // 想定燃焼時間 [ms]
  constexpr uint32_t DEFAULT_BURN_TIME_ms = 1350;
  uint32_t burn_time_ms = config::DEFAULT_BURN_TIME_ms;

  // 分離保護時間 [ms]
  constexpr uint32_t DEFAULT_SEPARATION_PROTECTION_TIME_ms = 7787;
  uint32_t separation_protection_time_ms = config::DEFAULT_SEPARATION_PROTECTION_TIME_ms;

  // 強制分離時間 [ms]
  constexpr uint32_t DEFAULT_FORCE_SEPARATION_TIME_ms = 11787;
  uint32_t force_separation_time_ms = config::DEFAULT_FORCE_SEPARATION_TIME_ms;

  // 想定着地時間 [ms]
  constexpr uint32_t DEFAULT_LANDING_TIME_ms = 45000;
  uint32_t landing_time_ms = DEFAULT_LANDING_TIME_ms;
}
```

分離用のコンフィグを定義しています。

定数になっている値はデフォルト値です。最終書き込み時点でのシミュレーション結果をプログラムに埋め込んでおきます。

`constexpr`  
ROM化される定数です。`#define`は使いたくないので頑なに定数を使っています。

#### 68~74行目

```cpp
namespace internal {
  FlightMode _flightMode;
  // 離床した瞬間の時間を保存する変数
  uint32_t _launchTime_ms;
  // 強制分離を実行したかを保存する変数
  bool _isForceSeparated = false;
}
```

フライトモードに関する雑多な変数たちを`internal`という名前空間で誤魔化しています。

`launchTime_ms`  
リフトオフ検知をした時に`millis()`で取得される起動時からの経過時間を保存します。リフトオフ検知に0.1秒かかるため、-100ms(-0.1s)をオフセットします。

`_isForceSeparated`  
強制分離は通常分離が行われたかに関わらず実行したいので、フライトモードがPARASHUTEでも実行可能判定になります。このままでは、強制分離が無限に実行され続けてしまいます。これを防ぐために強制分離が実行されると`_isForceSeparated`に`true`を代入して再度実行されることを防いでいます。

#### 76~92行目

```cpp
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
```

フライトデータを保存します。

|データ|単位|計測方法|
|-|-|-|
|気温|°C|BME280|
|気圧|Pa|BME280|
|高度|m|気温,気圧より算出|
|加速度 X軸|m/s^2|MPU6050|
|加速度 Y軸|m/s^2|MPU6050|
|加速度 Z軸|m/s^2|MPU6050|
|角速度 X軸|deg/s|MPU6050|
|角速度 Y軸|deg/s|MPU6050|
|角速度 Z軸|deg/s|MPU6050|
|姿勢角 ヨー|deg|加速度,角速度より算出|
|姿勢角 ピッチ|deg|加速度,角速度より算出|
|姿勢角 ロール|deg|加速度,角速度より算出|
|電圧 3.3V回路|V|ADC|
|電圧 5V供給|V|ADC|
|電圧 12V供給|V|ADC|

#### 95~121行目

```cpp
void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial1.begin(115200);

  // 7番ピンが短絡されていたらデータダンプを開始
  if (!device::_dataDumpModePin.isOpenActually()) {
    while (!Serial);
    device::_logger.dumpLog();
  }

  // LoRa 40ch
  LoRa.begin(923.8E6);

  downlinkEvent("INITIALIZE");

  device::_altimeter.initialize();
  device::_imu.initialize();

  Tasks.add(mainRoutine)->startIntervalMsec(10);
  Tasks.add(downlinkRoutine)->startIntervalMsec(500);
  Tasks.add("TurnOffShiranuiTask", turnOffShiranuiTask);

  MsgPacketizer::subscribe(LoRa, 0xF3, [](uint8_t command, float payload) {executeCommand(command, payload);});

  reset();

  downlinkEvent("START");
}
```

Arduino固有の`setup()`関数です。起動時に一度だけ実行されるので初期化に使います。

色々やってるので細分化して解説します。

```cpp
  Wire.begin();
  Serial.begin(115200);
  Serial1.begin(115200);
```

I2CとUARTを初期化します。`Wire`がI2C、SerialがUSB接続のUART、Serial1がSDカードロガー接続のUARTです。

```cpp
  // 7番ピンが短絡されていたらデータダンプを開始
  if (!device::_dataDumpModePin.isOpenActually()) {
    while (!Serial);
    device::_logger.dumpLog();
  }
```

起動モードを分岐するための処理です。

`device::_dataDumpModePin`はプルアップ設定の7番ピンのインスタンスです。`isOpenActually()`はノイズ除去(10回連続検知)を行わずにピンの現在の状態を返します。

if文の判定がtrueの時はSerial(USB接続)を待ってから`dumpLog()`を実行します。ログはSerialで出力されます。

```cpp
  // LoRa 40ch
  LoRa.begin(923.8E6);
```

LoRaを初期化します。引数の周波数に注意してください。  
[LoRaWANと９２０MHｚのルール | Qiita](https://qiita.com/ammo0613/items/d952154f1195b64dc29f)

```cpp
  downlinkEvent("INITIALIZE");
```

ダウンリンクでイベントとして**INITIALIZE**を送信します。`downlinkEvent("START");`も同じです。

```cpp
  device::_altimeter.initialize();
  device::_imu.initialize();
```

センサを初期化します。

```cpp
  Tasks.add(mainRoutine)->startIntervalMsec(10);
  Tasks.add(downlinkRoutine)->startIntervalMsec(500);
  Tasks.add("TurnOffShiranuiTask", turnOffShiranuiTask);
```

3つのタスクを初期化します。

1個めは`mainRoutine`で100Hzです。`mainRoutine`内容は[後述](#129153行目)します。

2個めは`downlinkRoutine`で2Hzです。`downlinkRoutine`内容は[後述](#156164行目)します。

3個目は`turnOffShiranuiTask`です。これは周期処理ではなく、分離から3秒後に実行されます。ONになった不知火をOFFにして、代わりにブザーをONにするためにあります。`turnOffShiranuiTask`の内容は[後述](#167170行目)します。

```cpp
  MsgPacketizer::subscribe(LoRa, 0xF3, [](uint8_t command, float payload) {executeCommand(command, payload);});
```

アップリンクのコマンドを購読します。LoRaでラベルが0xF3のパケットを受信した時に`executeCommand()`を実行します。詳細はMsgPacketizerの説明を参照してください。

#### 124~126行目

```cpp
void loop() {
  Tasks.update();
}
```

Arduino固有の`loop()`関数です。

すでにタスクは処理周期を指定して登録してあるので、`update()`するだけです。

#### 129~153行目

```cpp
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
    downlinkEvent("RESET");
    reset();
    device::_logger.initialize();
    changeFlightMode(FlightMode::STANDBY);
  }

  if (canSeparateForce()) {
    downlinkEvent("FORCE-SEPARATE");
    separate();
    internal::_isForceSeparated = true;
    changeFlightMode(FlightMode::PARACHUTE);
  }
}
```

100Hzで実行するメインルーチンです。`setup()`でタスクとして登録されます。

- フライトピンの状態を更新する
- センサからデータを読み出してフライトデータを更新する
- LED表示を更新する
- フライトモードを更新する
- ログを書き込む
- アップリンク用を受信していれば購読を更新する
- リセットが実行可能判定なら実行する
- 強制分離が実行可能判定なら実行する

#### 156~164行目

```cpp
void downlinkRoutine() {
  downlinkStatus();
  downlinkFlightData();
  downlinkConfig();

  device::_commandIndicator.on();
  device::_telemeter.sendDownlink();
  device::_commandIndicator.off();
}
```

2Hzで実行するダウンリンク送信用のルーチンです。`setup()`でタスクとして登録されます。

- ステータスを送信キューに入れる
- フライトデータを送信キューに入れる
- コンフィグを送信キューに入れる
- 送信キューの内容を送信する

#### 167~170行目

```cpp
void turnOffShiranuiTask() {
  device::_shiranui3.off();
  device::_buzzer.on();
}
```

分離から3秒後に実行する処理です。`setup()`でタスクとして登録されます。

- 不知火OFFにする
- ブザーをONにする

#### 173~195行目

```cpp
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

  // 測定電圧 = AD測定値 / 分解能 * 参照電圧 * 電圧係数
  // 電圧係数 = 最大電圧 / 基準電圧
  flightData::_voltage33 = analogRead(A6) / 1024.0 * 3.3 * 1.37;
  flightData::_voltage5 = analogRead(A5) / 1024.0 * 3.3 * 2.08;
  flightData::_voltage12 = analogRead(A4) / 1024.0 * 3.3 * 5.00;
}
```

センサのデータを読み出します。

電圧測定は少し複雑なことをしているので解説します。

まず、測定したい電圧は3.3V,5V,12Vの3種類です。これらを最大電圧とします。Arduino MKR WAN 1310のADC(Analog Digital Converter)は3.3Vまでしか入力できないため、3つの電圧を参照電圧以下に下げる必要があります。ここでは、この電圧を2.4Vの基準電圧とします。

※ADCの破損を防ぐため、最大電圧以上の電圧が入力されないことを回路上で担保してください。

3.3V,5V,12VをADCへ入力するために2.4Vへ降圧します。これに分圧回路を使います。

[抵抗分圧回路 | 電子回路設計 入門サイト](https://www.kairo-nyumon.com/resistor_divider.html)

また、基準電圧の2.4Vを最大電圧の3.3V,5V,12Vに戻すための係数を電圧係数として以下の関係式から求めます。

$$ 電圧係数 = \frac{最大電圧[\text{V}]}{基準電圧[\text{V}]} $$

`analogRead(ピン番号)`で計測値を取得できますが、実際の電圧に変換するためには以下の計算をする必要があります。

$$ 測定電圧[\text{V}] = \frac{1}{2^{分解能[\text{bit}]}} * 参照電圧[\text{V}] * 電圧係数 $$

Arduino MKR WAN 1310の仕様では、  
分解能 = 10[bit]  
参照電圧 = 3.3[V]  
です。

#### 198~203行目

```cpp
/// @brief 状況によってLEDを切り替える
void updateIndicators() {
  device::_protectionIndicator.set(isFlying() && millis() < internal::_launchTime_ms + config::separation_protection_time_ms);
  device::_flightIndicator.set(isFlying());
  device::_separationIndicator.set(internal::_flightMode == FlightMode::PARACHUTE);
}
```

基板に実装された3つの状態表示用LEDを切り替えます。

|LED|点灯条件|
|-|-|
|ProtectionIndicator</br>(分離保護表示)|飛行中</br>かつ 分離保護時間未経過|
|FlightIndicator</br>(飛行表示)|飛行中|
|SeparationIndicator</br>(分離表示)|フライトモードがPARASHUTE|

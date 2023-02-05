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

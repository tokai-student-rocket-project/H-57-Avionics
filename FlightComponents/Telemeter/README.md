# 目次

- [目次](#目次)
- [概要](#概要)
    - [目的](#目的)
    - [特徴](#特徴)
    - [成功基準](#成功基準)
- [H-57](#h-57)
  - [テレメトリー基板](#テレメトリー基板)
    - [仕様（テレメトリー基板）](#仕様テレメトリー基板)
    - [作業手順書（テレメトリー基板）](#作業手順書テレメトリー基板)
  - [バルブコントローラー基板](#バルブコントローラー基板)
    - [仕様（バルブコントローラー基板）](#仕様バルブコントローラー基板)
    - [作業手順書（バルブコントローラー基板）](#作業手順書バルブコントローラー基板)

# 概要

### 目的

- 酸化剤供給用バルブと主流路用バルブの制御を行う
- 位置情報を取得しロケットの現在地を地上に送信する

### 特徴

- 使用するマイコンボードにArduinoを採用
  
  → プログラミング初学者でも比較的作成が容易！

  → 拡張性・運用性・保守性が向上！
- LoRaWANの搭載
  - 簡単なプログラムで見通し10km以上の通信が可能
  
### 成功基準
![工事中](https://p100k.jp/wp-content/uploads/2021/03/EI4vUVMUYAAZzj7-1024x905-1-1.jpg "工事中")

[目次へ戻る](#目次)
***

# H-57

## テレメトリー基板
### 仕様（テレメトリー基板）
- ハードウェア
  - マイコンボード
    - [MKR WAN 1310](https://docs.arduino.cc/hardware/mkr-wan-1310)（Arduino公式HPより）
  - 動作電圧
    - USB給電：5.0V
    - 外部給電：5.0V
  - Shield
    - [MKR GPS Shield](https://docs.arduino.cc/hardware/mkr-gps-shield?_gl=1*mc5mbr*_ga*MTQ0MTM0MzY4MS4xNjYyMDAyMTMy*_ga_NEXN8H46L5*MTY3NTIyOTgwNi44OC4xLjE2NzUyMzA4NTguMC4wLjA.)（Arudino公式HPより）
      - バックアップ電池：CR1216
- ソフトウェア


  - 使用したライブラリ
    - [Arduino_MKRGPS.h](https://github.com/arduino-libraries/Arduino_MKRGPS)
      - MKR GPS Shield用のライブラリです。緯度、経度だけでなく受信した衛星の数や速度も取得できます。
    - [LoRa.h](https://github.com/sandeepmistry/arduino-LoRa)
      - LoRa(Low Power Wide Area)の一種。通信規格のこと。
      - LoRaで通信する際に必要なライブラリ
    - [SPI.h](https://www.arduino.cc/reference/en/language/functions/communication/spi/)
      - SPI通信をする際に必要なライブラリ
    - [ArduinoJson.h](https://github.com/bblanchon/ArduinoJson)
      - [公式HP](https://arduinojson.org/)
      - Json形式で値のやり取りができるようになるライブラリ
  - [**Inside.ino**](https://github.com/tokai-student-rocket-project/Avionics/tree/main/FlightComponents/Telemeter/Inside)
    - H-57に搭載するテレメトリー基板に使用するソースコード（名前がダサいのは直すかもしれないです...）


- 取得するデータ
  - 緯度
  - 経度
  - Epoch Time
  - 供給用バルブの角度
  - 主流路バルブの角度

### 作業手順書（テレメトリー基板）
- 事前準備


  1. USB-MicroB - 自身のPCにあう端子のケーブル（以後、USB-MicroBケーブル）、***MKR WAN 1310***、***MKR GPS Shield***を用意してください。
  2. USB-MicroB ケーブルを***MKR WAN 1310***に接続してください。 
  3. [Inside.ino](https://github.com/tokai-student-rocket-project/Avionics/tree/main/FlightComponents/Telemeter/Inside) を ***MKR WAN 1310*** へ書き込んで下さい。書き込み後、USB-MicroBケーブルを取り外してください。
  4. 基板へ1で書き込んだ ***MKR WAN 1310*** を**向きに気をつけて**取り付けてください。
  5. ***MKR GPS Shield*** を基板へ向きに気をつけて取り付けてください。


- 組み立て
  - [こちら](./docs/Manual_Telematar.md)を参照してください。

[目次へ戻る](#目次)
***

## バルブコントローラー基板
### 仕様（バルブコントローラー基板）
- ハードウェア
  - マイコンボード
    - [ARDUINO NANO](https://docs.arduino.cc/hardware/nano?_gl=1*huj9p8*_ga*MTQ0MTM0MzY4MS4xNjYyMDAyMTMy*_ga_NEXN8H46L5*MTY3NTIyOTgwNi44OC4xLjE2NzUyMzAwODUuMC4wLjA.)（Arduino公式HPより）

  - 動作電圧
    - USB給電:5.0V
    - 外部給電:5.0V
  - アクチュエータ
    - [RS-405CB](https://www.futaba.co.jp/product/robot/command_type_servos/rs405cb)（Futaba公式HPより）
      - 電源電圧:11.1[V]
      - トルク:48.0[kgf.cm]
      - スピード:0.22[s/60°]
      - 可動範囲:300[deg]
- ソフトウェア
  - [VerSpeedServo](https://github.com/netlabtoolkit/VarSpeedServo)
    - アクチュエータの角度、速度を制御します。


  ![工事中](https://p100k.jp/wp-content/uploads/2021/03/EI4vUVMUYAAZzj7-1024x905-1-1.jpg "工事中")

### 作業手順書（バルブコントローラー基板）
- [こちら](./docs/Manual_ValveContoroler.md)を参照してください。

[目次へ戻る](#目次)

***

![工事中](https://media.tenor.com/qEtHVSXW5ZkAAAAC/genba-neko-cat.gif "工事中")
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
  →　プログラミング初学者でも比較的作成が容易
  → 拡張性・運用性・保守性が向上
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
- ソフトウェア
  - [Arduino_MKRGPS](https://github.com/arduino-libraries/Arduino_MKRGPS)
    - MKR GPS Shield用のライブラリです。緯度、経度だけでなく受信した衛星の数や速度も取得できます。
![工事中](https://media.tenor.com/qEtHVSXW5ZkAAAAC/genba-neko-cat.gif "工事中")

### 作業手順書（テレメトリー基板）
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
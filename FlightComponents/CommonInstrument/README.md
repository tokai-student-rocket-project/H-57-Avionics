# 新型共通計器

## 概要

## 仕様

### ハードウェア

詳細は[こちら](./Hardware/README.md)を参照してください。

### 計測データ

|データ|取得方法|
|-|-|
|加速度|MPU6050|
|角速度|MPU6050|
|姿勢角|加速度,角速度から算出|
|気圧|BME280|
|気温|BME280|
|高度|気圧,気温から算出|

サンプリングレートは全て100Hzです。

#### 姿勢角算出

Magdwickフィルタで算出します。
論文（["An efficient orientation filter for inertial and inertial/magnetic sensor arrays"](https://www.samba.org/tridge/UAV/madgwick_internal_report.pdf))もありますが、細かいことはわかりません。細かいことがわからなくてもライブラリがあるので大丈夫です。

ライブラリ  
[MagdwickAHRS(Github)](https://github.com/arduino-libraries/MadgwickAHRS)  
ライブラリの使い方  
[ArduinoのMadgwickライブラリの使い方 – imo Lab.](https://garchiving.com/how-to-madgwick-library-of-arduino/)

MPU6050にはDMP(Digital Motion Processor)という内部処理機構があり、ライブラリ([MPU6050_6Axis_MotionApps20](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050))を使うことで簡単に姿勢角を算出することができますが、これ±2Gまでしか対応していないみたいです。なので代わりにMagdwickフィルタを使いました。

#### 高度算出

気圧センサのBME280を抽象化したライブラリを使っています。この中に`readFloatAltitudeMeters()`があるので簡単に高度を算出できます。あらかじめ`setReferencePressure(float refPressure)`で基準気圧を設定する必要があります。デフォルトでは起動してから2秒後に計測した気圧を基準気圧として設定します。

ライブラリの中の高度を算出している式です。

```cpp
heightOutput = ((float)-44330.77)*(pow(((float)readFloatPressure()/(float)_referencePressure), 0.190263) - (float)1);
```

数式に直すとこんな感じ。

$$ h = -44330.77 \times \left\{ \left( \frac{P}{P_{0}} \right) ^{0.190263} - 1 \right\} \\ h : \text{高度[m]}, P : \text{計測気圧[Pa]}, P_{0} : \text{基準気圧[Pa]} $$

["International barometric height formula"](http://hyperphysics.phy-astr.gsu.edu/hbase/Kinetic/barfor.html)を使っているみたいです。対流圏(高度0kmから11km)で有効です。

ライブラリ  
[SparkFun_BME280_Arduino_Library(Github)](https://github.com/sparkfun/SparkFun_BME280_Arduino_Library)

気圧を取得する時は`readFloatPressure()`の前に`readTempC()`を呼び出す必要があります。

### フライトモード

### 分離ロジック

分離判定は**頂点検知+タイマー**です。

![論理回路図](./Documents/SafetyReview/LogicDiagram.png)

#### 頂点検知

高度が連続で減少したことを検知して頂点とします。

気圧測定 → 高度算出 → 指数移動平均 → 連続降下検知 → 頂点検知

<指数移動平均>
[高度算出](#高度算出)で得た高度にはノイズが含まれているため指数移動平均で平滑化します。

$$ \mu = \mu_{ref} + \alpha \times \left( h - \mu_{ref} \right) \\ \mu : \text{平均[m]}, \mu_{ref} : \text{前回の平均[m]}, \alpha : \text{平滑化定数}, h : \text{高度[m]} $$

平滑化は任意に設定する必要があります。0~1の範囲で大きいほど平滑化が強くなります。地上での試験では0.35が適切でしたが、こればっかりは打ち上げないとわからないです。

指数移動平均は前回の値1つを保存しておけば良いので、単純移動平均と比較して負荷が軽いという特徴があります。

<連続降下検知>
平滑化した高度が連続で減少した回数をカウントします。連続なので1度でも減少しなければカウントはリセットされます。開発時は10回連続にしていますが、平滑化定数との兼ね合いもあるのでこればっかりは打ち上げないとわからないです。

<頂点検知>
上昇中（[フライトモード](#フライトモード)がCLIMB）に連続降下検知がされた場合はフライトモードをDESCENTモードに変更します。同時にタイマーが分離保護時間を経過すれば、分離指令を出してフライトモードをPARASHUTEモードに変更します。

#### タイマー

タイマーではバックアップや検知の補助のために以下の時間を設定します。

- 想定燃焼時間
- 分離保護時間
- 強制分離時間
- 想定着地時間
  
<想定燃焼時間>  
モータの燃焼時間を設定します。今後、燃焼終了検知を実装した時の補助になります。

<分離保護時間>  
燃焼中や速度が速い段階で分離してしまうことを防ぎます。想定される頂点到達時間の-1~2秒を設定します。

<強制分離時間>  
落下中にパラシュートの許容速度以上に加速してしまうことを防ぎます。想定される頂点到達時間の+1~2秒を設定します。

<想定着地時間>  
ログの保存などの処理を終了する時間です。

#### 分離指令

不知火の電磁弁([HNB1-M5-DC12V](https://jp.misumi-ec.com/vona2/detail/221000160046/))にDC12Vを3秒間供給します。信号の切り替えにはフォトリレー([TLP225A](https://akizukidenshi.com/catalog/g/gI-12192/))を使います。

DC12Vを不知火へ3秒間供給した後はブザーへ供給します。これは機体回収用でバッテリーが尽きるまで鳴り続きます。

## 運用方法

### 起動モード

Arduinoの指定のピンとGNDを短絡させた状態で起動することで起動モードを変更できます。

※DATA_DUMP_MODEのピンソケットは配線ミスで使えません。

#### ログ出力モード

**D7**ピンと**GND**を短絡

EEPROMに保存されたログをシリアル通信で出力します。ArduinoのUSBとPCを接続してシリアルモニタを起動してください。

|ボーレート|改行コード|
|-|-|
|115200|LF|

ログはcsv形式です。

```text:sample.csv
flightTime,flightMode,stateShiranui3,stateBuzzer,pressure,temperature,altitude,descentCount、accelerationX,accelerationY,accelerationZ,gyroX,gyroY,gyroZ,yaw,pitch,roll
0.10,1,0,0,101394.73,21.35,0.63,0,0.00,0.01,1.01,-0.37,0.79,-1.59,100.20,0.53,-0.70
0.12,1,0,0,101399.08,21.36,0.27,1,-0.00,-0.00,1.02,-0.24,0.85,1.40,100.21,0.48,-0.61
0.13,1,0,0,101404.27,21.34,-0.16,2,-0.01,-0.02,1.01,-0.43,0.85,1.52,100.22,0.53,-0.71
0.15,1,0,0,101397.33,21.36,0.42,0,-0.01,-0.02,1.00,-0.55,0.73,1.65,100.24,0.45,-0.79
0.17,1,0,0,101402.52,21.37,-0.01,1,-0.01,-0.01,1.01,-0.37,0.91,1.71,100.26,0.57,-0.75
```

#### ログ無しモード

**D8**ピンと**GND**を短絡

EEPROMとSDロガーへのログ保存を行わないモードです。EEPROMの書き込み回数を温存したい時に使用します。

---

## 生産者表示

```text
                                    ....
                              .dMMMMMMMMMNa,,
                             .MMMMMMMMMMMMMMMML
                            dMMMMMMMMMMMMMMMMMMh
                           ,MMMMBwtttwXWHMMMMMMMb
                           (MMM8rrtttttrwUHMMMMMM[
                           -MM#zzwOOzzOwwwwXMMMMMM;
                           .MMHWHMMMkXXWHHUXUXWMMMN
                            MMuXQQNkUZwXWMMMNXudMM#
                            JMuVwvzwOC+7COwrrrzuMMF
                             qzZ11>>>>;>>>>>>?vVWWL
                             (Oz?>>??>><>;>>>?=OWSI
       私が作りました>       .v=?1>??====<>+1??+kX'
                              !z11??=11?????++=d\
                               (Orzlll=???=zz=zO
                               .?OOv1??====zvOd}
                                  ?Zwz=llzOzzXXI
                                   XkQqHHkWWWXuuMMM;
                                   mWWHHmHWuzzzuqNMMMMN,
                               .dMMNmyVWHWyXXXQMMMMMMMMMNg,
                               MMMMMMMkWVVWqMMMMMMMMMMMMMMMMNJ.
                            .gMMMMMMMMMMNMMMMMMMMMMMMMMMNkwtMMMMa,.
                        ..MMMMMMMMMMMNMNMMMMMMMMMMMMMMMMMMmXr?MMMMMN,
                     .MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNOlzMMMMMM,
             ..    .MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNwtlTEOlXM,
           .lO2   .MMMMMMMMMMMMMMMMMMMMMMMNMMMMMMMMMMMMMBAZ+WWwttXwOlON.
          .tOZ`  .MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNQkQOrwWXttWwllvN
        ..lzZ`  .MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMRwOtwXXttlllllL
     JlzOlzd^   dMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKlOOtOwvOllltlw;
    ==zOlzw! . .MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNOtltOlOrOzOQkNN,
  .1=zZlzwynOlOMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM#XwQmmQQQQNRWNMMb
 .===zlOwZO=lz1=dMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMHWMHMMMMMMMMMMMMp
 OllllttOllzv==zMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNMHMMMMMMMMMMMMM|
 .4kzzzl=lzz=zzwMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM#MMMMMMMMMMMM,
   ,MNmgmQQmmkkXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMb
    .MMMMMMH@HHHMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM,
    ,MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM%
     ?MMNHqHMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM;
      MMMMNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM#db
```

---

©️ 1999 Tokai Student Rocket Project

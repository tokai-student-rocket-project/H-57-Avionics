# 新型共通計器

## 概要

## 仕様

### ハードウェア

詳細は[こちら](./Hardware/README.md)を参照してください。

### 起動モード

Arduinoの指定のピンとGNDを短絡させた状態で起動することで起動モードを変更できます。  
※DATA_DUMP_MODEのピンソケットは配線ミスで使えません。

#### ログ出力モード

**D7**ピンと**GND**を短絡

EEPROMに保存されたログをシリアル通信で出力します。ArduinoのUSBをPCを接続してシリアルモニタを起動してください。

|ボーレート|改行コード|
|-|-|
|115200|LF|

ログはcsv形式です。

```text サンプル
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

## 運用方法

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

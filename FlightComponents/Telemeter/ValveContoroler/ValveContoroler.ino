/*
  Writer?:Hiroki TSUTSUMI(1CES2116)
  Rocket:H-57
  MCU Series:Arduino
  MCU:SAMD21

  バルブ位置を制御し、テレメトリーモジュールに送信する機能を持っています。
*/


#include "VarSpeedServo.h"
#include <ArduinoJson.h>

constexpr int POSITION_CHANGING_THRESHOLD = 10;

int LCount = 0;
int WCount = 0;

// 1: Waiting, 2: Launch
int Position = 1;

// Servo設定
VarSpeedServo Mainservo;
VarSpeedServo Supplyservo;
//int MainServoPin = 5; // Uno
int MainServoPin = 9; // Nano
//int SupplyServoPin = 6; // Uno
int SupplyServoPin = 10; // Nano
float Supplyservo_deg;
float Mainservo_deg;

void setup()
{
    // Serial.begin(115200);
    //  while (!Serial)
    //      continue;
    Serial.begin(9600);
    // Serial.begin(115200);

    Mainservo.attach(MainServoPin);
    Mainservo.write(20, 30, true); // 不感体に近い為初期値を20度ずらした。 0 -=> 20, 140 -=> 160
    Supplyservo.attach(SupplyServoPin);
    Supplyservo.write(20, 30, true); // 不感体に近い為初期値を20度ずらした。 0 -=> 20, 60 -=> 80

    pinMode(2, INPUT_PULLUP); // WAITING <=- Arduino UNO　//LUNCH <=- Arduino NANO　//ただの設計ミス。ソフトで解決可能なのでUNO,NANOで切り替えてほしい...
    pinMode(3, INPUT_PULLUP); // LUNCH <=- ArduinoUNO //WAITING <=- Arduino NANO
    pinMode(LED_BUILTIN, OUTPUT);

    delay(1000);
}

void loop()
{
    // WaitingポジションかつLaunch信号がHIGHならLCountを加算する。それ以外ならLCountを0にリセットする
<<<<<<< HEAD
    // if (Position == 1 && digitalRead(2) == LOW) //Arduino UNO 簡易的に作成した制御シールド用
    if (Position == 1 && digitalRead(3) == LOW) // Arduino NANO　H-57 仕様
=======
    // if (Position == 1 && digitalRead(2) == LOW) // UNO
    if (Position == 1 && digitalRead(3) == LOW) // NANO
>>>>>>> 33085ccc91ba07cb07be3134e8f1b1eef6d22198
    {
        LCount++;
    }
    else
    {
        LCount = 0;
    }

    // LCountが閾値以上になればLaunchポジションに変更する
    if (LCount >= POSITION_CHANGING_THRESHOLD)
    {
        LCount = 0;

        L_Position();
        Position = 2;
        delay(300);
    }

    // 以下、WaitingとLaunchが逆になったバージョン
<<<<<<< HEAD
    // if (Position == 2 && digitalRead(3) == LOW) //Arduino UNO 簡易的に作成した制御シールド用
    if (Position == 2 && digitalRead(2) == LOW) // Arduino NANO　H-57 仕様
=======
    // if (Position == 2 && digitalRead(3) == LOW) // UNO
    if (Position == 2 && digitalRead(2) == LOW) // NANO
>>>>>>> 33085ccc91ba07cb07be3134e8f1b1eef6d22198
    {
        WCount++;
    }
    else
    {
        WCount = 0;
    }

    if (WCount >= POSITION_CHANGING_THRESHOLD)
    {
        WCount = 0;

        W_Position();
        Position = 1;
        delay(300);
    }

<<<<<<< HEAD
    // Arduinojson
    StaticJsonDocument<64> servoPacket;
    servoPacket["mainservoDeg"] = Mainservo_deg;
    servoPacket["supplyservoDeg"] = Supplyservo_deg;
=======
    //
<<<<<<< HEAD
    //StaticJsonDocument<64> servoPacket;
    //servoPacket["mainservoDeg"] = Mainservo_deg;
    //servoPacket["supplyservoDeg"] = Supplyservo_deg;
>>>>>>> 33085ccc91ba07cb07be3134e8f1b1eef6d22198
=======
    StaticJsonDocument<64> servoPacket;
    servoPacket["mainservoDeg"] = Mainservo_deg;
    servoPacket["supplyservoDeg"] = Supplyservo_deg;
>>>>>>> 1c0a7494f0457d85543bff60ddf13c99d63a2a63

    Serial.println();
    serializeJson(servoPacket, Serial);

<<<<<<< HEAD
<<<<<<< HEAD
    // Arduino IDE シリアルプロッタでのデバック用
    //  Serial.print(WCount);
    //  Serial.print(",");
    //  Serial.println(LCount);
=======
    Serial.print(WCount);
    Serial.print(",");
    Serial.println(LCount);
>>>>>>> 33085ccc91ba07cb07be3134e8f1b1eef6d22198
=======
    //Serial.print(WCount);
    //Serial.print(",");
    //PSerial.println(LCount);
>>>>>>> 1c0a7494f0457d85543bff60ddf13c99d63a2a63

    // delay(2); // 250Hz
    delay(10);
}
void L_Position()
{
    Supplyservo.write(80, 255, true); // SupplyServo CLOSE //0がMAX Speed Fill弁が閉まる時間:???s
    // Supplyservo.write(60, 80, true); //Fill弁が閉まる時間:0.54s

<<<<<<< HEAD
    delay(10); //
=======
    delay(100);
>>>>>>> 1c0a7494f0457d85543bff60ddf13c99d63a2a63

    // Mainservo.write(140, 50, true); // MainServo OPEN //Main弁が閉まる時間:1.22s //失敗
    // Mainservo.write(160, 160, true); // Mainservo OPEN //Main弁が閉まる時間: UNO

    // Memo Sweagelock  45 60
    // 2023/1/21 フローテック -=> Sweagelock にバルブ変更
    Mainservo.write(57, 40, true); // UNO //NANO

    digitalWrite(LED_BUILTIN, HIGH);

    Supplyservo_deg = Supplyservo.read();
    Mainservo_deg = Mainservo.read();
    // 2022/12/21
    // コールドフロー試験
    // Mainservo.write(140, 30, true); 角度140°、速度30にて成功
    // 成功を踏まえて...速度を早くするべきかどうか検討する必要あり。
    //
    // 2022/12/23
    // 燃焼実験
    // Mainservo.write(140,50, true);角度140°、速度50で実験
    // 点火成功！
    //
    // 2023/1/11
    // コールドフロー試験
    // Supplyservo.write(60, 60, true);
    // Supplyservo_deg = Supplyservo.read();
    // delay(10);
    // Mainservo.write(140, 50, true);
    // Mainservo_deg = Mainservo.read();
    // digitalWrite(LED_BUILTIN, HIGH);
    // 上記の動作でコールドフロー試験を成功させることが出来た。やったね！
}

void W_Position()
{
    Supplyservo.write(20, 30, true); // SupplyServo OPEN
    delay(10);
    Mainservo.write(20, 30, true); // MainServo CLOSE

    digitalWrite(LED_BUILTIN, LOW);
    Supplyservo_deg = Supplyservo.read();
    Mainservo_deg = Mainservo.read();
}

/*
                                    .[
                                 .dMM@
                 ...&gNMMMMNNgJ+MMMMMN
             ..MMMMH"""7??77""MMMMMMMM;
          .JMMMMMMMMMMMMNg,. .MMMMMMMMN,
        .MMMWMMMMMMMMMMMMMMMNMMMMMMMM"WMN,
      .dM#^JM#"         ?TMMMMMMMMMM^  .WMN,
     .MMt JM^              .MMMMMMM'     ,MMb
    .MM! .@               .MMMMMMMMb       UMN.
   .M#`  d`               dMMMMMMMMMN,      UMN
  .MM!   F               .MMMMM#`,MMMM,      HMb
  MMF    \    ...       .MMMMM#`  .MMMM,     ,MM.
 .MM`    i.MMMMMMMMNJ.  dMMMM#     .MMMN.     MM]
 -M#   .dMMMMMMMMMMMMMN.MMMM@       ,MMMb     JM@
 (M#  .MMMMMMMMMMMMMMM# TMM@         dMMM;    (M#
 -M# .MMMMMMMMMMMMMMMM'  WF          .MMMb    JMF
 .MM.dMMMMMMMMMMMMMMMF   .            MMMN    MM]
  dMbMMMMMMMMMMMMMMM@   .#            MMMN   .MM
  .MMdMMMMMMMMMMMMMM!  .MF            MMM#  .MMt
   -MMMMMMMMMMMMMMMF  .M@            .MMMF .MMF
    (MMMMMMMMMMMMM@  .MD            .MMMM`.MMF
     ,MMNTMMMMMMMM! ,"             .MMMM\.MM3
       TMMa._7"""QN"(.           ..MMMMQMM@`
         TMMN,  .M3   74J......(MMMMMMMM@!
           ?WMMNML.       ?"""HHMNMMM#"
              (MHMMMMNggg&ggNMMMMM9^
              .^   -?""""""""7!
*/

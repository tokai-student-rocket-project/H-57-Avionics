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

    pinMode(2, INPUT_PULLUP); // WAITING
    pinMode(3, INPUT_PULLUP); // LUNCH
    pinMode(LED_BUILTIN, OUTPUT);

    delay(1000);
}

void loop()
{
    // WaitingポジションかつLaunch信号がHIGHならLCountを加算する。それ以外ならLCountを0にリセットする
    // if (Position == 1 && digitalRead(2) == LOW) // UNO
    if (Position == 1 && digitalRead(3) == LOW) // NANO
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
    // if (Position == 2 && digitalRead(3) == LOW) // UNO
    if (Position == 2 && digitalRead(2) == LOW) // NANO
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

    //
    //StaticJsonDocument<64> servoPacket;
    //servoPacket["mainservoDeg"] = Mainservo_deg;
    //servoPacket["supplyservoDeg"] = Supplyservo_deg;

    // Serial.println();
    // serializeJson(servoPacket, Serial);

    Serial.print(WCount);
    Serial.print(",");
    Serial.println(LCount);

    // delay(2); // 250Hz
    delay(10);
}
void L_Position()
{
    Supplyservo.write(80, 0, true); // SupplyServo CLOSE //0がMAX Speed Fill弁が閉まる時間:???s
    // Supplyservo.write(60, 80, true); //Fill弁が閉まる時間:0.54s

    delay(10);

    // Mainservo.write(140, 50, true); // MainServo OPEN //Main弁が閉まる時間:1.22s //失敗
    // Mainservo.write(160, 160, true); // Mainservo OPEN //Main弁が閉まる時間: UNO

    // Memo Sweagelock  45 60
    // 2023/1/21 フローテック -=> Sweagelock にバルブ変更
    Mainservo.write(57, 0, true); // UNO //NANO

    digitalWrite(LED_BUILTIN, HIGH);

    Supplyservo_deg = Supplyservo.read();
    Mainservo_deg = Mainservo.read();
    // 2022/12/21
    // コールドフロー試験
    // Mainservo.write(140, 30, true); 角度140°、速度30にて成功
    // 成功を踏まえて...
    // 速度を早くするべきかどうか検討する必要あり。
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
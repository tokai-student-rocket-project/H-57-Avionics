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
int MainServoPin = 5; // Uno
// int MainServoPin = 5; //Nano
int SupplyServoPin = 6; // Uno
// int SupplyServoPin = 6; //Nano
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
    Supplyservo.attach(SupplyServoPin);

    pinMode(2, INPUT_PULLUP); // WAITING
    pinMode(3, INPUT_PULLUP); // LUNCH
    pinMode(LED_BUILTIN, OUTPUT);

    delay(1000);

    Mainservo.write(0, 10, true);
    Supplyservo.write(0, 30, true);
}

void loop()
{
    // WaitingポジションかつLaunch信号がHIGHならLCountを加算する。それ以外ならLCountを0にリセットする
    if (Position == 1 && digitalRead(2) == LOW)
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
    if (Position == 2 && digitalRead(3) == LOW)
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
    StaticJsonDocument<64> servoPacket;
    servoPacket["mainservoDeg"] = Mainservo_deg;
    servoPacket["supplyservoDeg"] = Supplyservo_deg;

    Serial.println();
    serializeJson(servoPacket, Serial);

    // Serial.print(WCount);
    // Serial.print(",");
    // Serial.println(LCount);

    delay(2); // 250Hz
    // delay(10);
}
void L_Position()
{
    /*燃焼、コールドフロー試験用にコメントアウト*/
    Supplyservo.write(60, 60, true); // SupplyServo CLOSE　//Fill弁が閉まる時間:0.54s
    Supplyservo_deg = Supplyservo.read();
    delay(10);

    /* --この下消去予定-- */
    // delay(450);
    // delay(500); //250Hz, delay(500), 点火処理時間:2.457s
    //  delay(1000); //250Hz, delay(1000), 点火処理時間:3.00s
    /* ----------------- */

    Mainservo.write(140, 50, true); // MainServo OPEN //Main弁が閉まる時間:1.22s
    Mainservo_deg = Mainservo.read();
    digitalWrite(LED_BUILTIN, HIGH);

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
    Supplyservo.write(0, 60, true); // SupplyServo OPEN
    Supplyservo_deg = Supplyservo.read();
    delay(1000);
    Mainservo.write(0, 30, true); // MainServo CLOSE
    Mainservo_deg = Mainservo.read();
    digitalWrite(LED_BUILTIN, LOW);
}
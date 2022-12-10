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
int MainServoPin = 5;
int SupplyServoPin = 6;
float Supplyservo_deg;
float Mainservo_deg;

StaticJsonDocument<32> ServoDeg;

void setup()
{
    Serial.begin(9600);
    Mainservo.attach(MainServoPin);
    Supplyservo.attach(SupplyServoPin);

    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);

    Mainservo.write(0, 30, true);
    Supplyservo.write(0, 30, true);
}

void loop()
{
    // WaitingポジションかつLaunch信号がHIGHならLCountを加算する。それ以外ならLCountを0にリセットする
    if (Position == 1 && digitalRead(2) == LOW) {
        LCount++;
    } else {
        LCount = 0;
    }

    // LCountが閾値以上になればLaunchポジションに変更する
    if (LCount >= POSITION_CHANGING_THRESHOLD) {
        LCount = 0;

        L_Position();
        Position = 2;
        delay(300);
    }

    // 以下、WaitingとLaunchが逆になったバージョン

    if (Position == 2 && digitalRead(3) == LOW) {
        WCount++;
    } else {
        WCount = 0;
    }

    if (WCount >= POSITION_CHANGING_THRESHOLD) {
        WCount = 0;

        W_Position();
        Position = 1;
        delay(300);
    }

    ServoDeg["mainservoDeg"] = String(Mainservo_deg, 2);
    ServoDeg["supplyservoDeg"] = String(Supplyservo_deg, 2);
    serializeJson(ServoDeg, Serial);

    delay(10);
}

void L_Position()
{
    Supplyservo.write(90, 60, true); //SupplyServo CLOSE
    Supplyservo_deg = Supplyservo.read();
    delay(1000);
    Mainservo.write(140, 30, true); //MainServo OPEN
    Mainservo_deg = Mainservo.read();
}

void W_Position()
{
    Supplyservo.write(0, 60, true); //SupplyServo OPEN
    Supplyservo_deg = Supplyservo.read();
    delay(1000);
    Mainservo.write(0, 30, true); //MainServo CLOSE
    Mainservo_deg = Mainservo.read();
}
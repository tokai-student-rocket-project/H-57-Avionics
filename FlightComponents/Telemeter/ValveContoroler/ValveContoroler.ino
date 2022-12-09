#include "VarSpeedServo.h"

constexpr int POSITION_CHANGING_THRESHOLD = 10;

int LCount = 0;
int WCount = 0;

// 1: Waiting, 2: Launch
int Position = 0;

VarSpeedServo Mainservo;
VarSpeedServo Supplyservo;
int MainServoPin = 5;
int SupplyServoPin = 6;

void setup()
{
    Serial.begin(9600);
    Mainservo.attach(MainServoPin);
    Supplyservo.attach(SupplyServoPin);

    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
}

void loop()
{
    // WaitingポジションかつLaunch信号がHIGHならLCountを加算する。それ以外ならLCountを0にリセットする
    if (Position == 1 && digitalRead(2) == HIGH) {
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

    if (Position == 2 && digitalRead(3) == HIGH) {
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

    delay(10);
}

void L_Position()
{
    Supplyservo.write(90, 60, true); //SupplyServo CLOSE
    delay(1000);
    Mainservo.write(140, 30, true); //MainServo OPEN
    Serial.println("LUNCH");
}

void W_Position()
{
    Supplyservo.write(0, 60, true); //SupplyServo OPEN
    delay(1000);
    Mainservo.write(0, 30, true); //MainServo CLOSE
    Serial.println("WAITING");
}







/*
#include "VarSpeedServo.h"
#include <ArduinoJson.h>


//Servo設定
VarSpeedServo Mainservo;
VarSpeedServo Supplyservo;
int MainServoPin = 5;
int SupplyServoPin = 6;
float Supplyservo_deg;
float Mainservo_deg;

constexpr int POSITION_CHANGING_THRESHOLD = 10;

volatile int LCount = 0;
volatile int WCount = 0;

//StaticJsonDocument<32> ServoDeg;

void setup()
{
    Serial.begin(9600);

    Mainservo.attach(MainServoPin);
    Supplyservo.attach(SupplyServoPin);

    pinMode(2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(2), L_SW, RISING);

    pinMode(3, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(3), W_SW, RISING);
}

void loop()
{
    if (LCount => POSITION_CHANGING_THRESHOLD) {
        LCount = 0;

        L_Position();
        delay(300);

        
        ServoDeg.clear();
        ServoDeg["mainservoDeg"] = String(Mainservo_deg, 2);
        ServoDeg["supplyservoDeg"] = String(Supplyservo_deg, 2);
        serializeJson(ServoDeg, Serial);
        
    }

    if (WCount => POSITION_CHANGING_THRESHOLD) {
        WCount = 0;

        W_Position();
        delay(300);

        
        ServoDeg.clear();
        ServoDeg["mainservoDeg"] = String(Mainservo_deg, 2);
        ServoDeg["supplyservoDeg"] = String(Supplyservo_deg, 2);
        serializeJson(ServoDeg, Serial);
        
    }

    delay(10);
}

void L_SW()
{
    LCount++;
    WCount = 0;
}

void W_SW()
{
    WCount++;
    LCount = 0;
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
*/


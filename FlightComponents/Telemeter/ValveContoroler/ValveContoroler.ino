#include "VarSpeedServo.h"
#include <ArduinoJson.h>


//Servo設定
VarSpeedServo Mainservo;
VarSpeedServo Supplyservo;
int MainServoPin = 5;
int SupplyServoPin = 6;
float Supplyservo_deg;
float Mainservo_deg;

volatile int t = 0;

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
    if (t == 1)
    {
        t = 0;
        L_Position();
        delay(300);
        
        /*
        ServoDeg.clear();
        ServoDeg["mainservoDeg"] = String(Mainservo_deg, 2);
        ServoDeg["supplyservoDeg"] = String(Supplyservo_deg, 2);
        serializeJson(ServoDeg, Serial);
        */
    }

    if (t == 2)
    {
        t = 0;
        W_Position();
        delay(300);
        
        /*
        ServoDeg.clear();
        ServoDeg["mainservoDeg"] = String(Mainservo_deg, 2);
        ServoDeg["supplyservoDeg"] = String(Supplyservo_deg, 2);
        serializeJson(ServoDeg, Serial);
        */
    }
}

void L_SW()
{
    t = 1;
}

void W_SW()
{
    t = 2;
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

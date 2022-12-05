#include "VarSpeedServo.h"

VarSpeedServo Mainservo;
VarSpeedServo Supplyservo;

int MainServoPin = 9;
int SupplyServoPin = 10;
float Supplyservo_deg;
float Mainservo_deg;

volatile int t = 0;

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
    Serial.println(Supplyservo_deg);
    Serial.println(Mainservo_deg);

    if(t == 1)
    {
        t = 0;
        L_Position();
        delay(10);
    }

    if (t == 2)
    {
        t = 0;
        W_Position();
        delay(10);
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
    Supplyservo.write(90, 60, true);
    Supplyservo_deg = Supplyservo.read();
    delay(1000);
    Mainservo.write(140, 30, true);
    Mainservo_deg = Mainservo.read();
}


void W_Position()
{
    Supplyservo.write(0, 60, true);
    Supplyservo_deg = Supplyservo.read();
    delay(1000);
    Mainservo.write(0, 30, true);
    Mainservo_deg = Mainservo.read();
}

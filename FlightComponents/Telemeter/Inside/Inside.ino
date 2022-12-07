/*
  機体からGPS情報を地上に送信するArduinoです。
  ソースファイル名：Inside.ino
  作成者：1CES2116 Hiroki Tsutsumi, <氏名の追加>

  H-57号機に搭載予定
*/

#include <LoRa.h>
#include <SPI.h>
#include <Arduino_MKRGPS.h>
#include <Servo.h>
#include <ArduinoJson.h>

//サーボの設定
//Servo mainservo;
//Servo supplyservo;
float supplyservo_deg;
float mainservo_deg;
//int supply_min_deg = 0;
//int supply_max_deg = 90;
// int main_min_deg = 0;
// int main_max_deg = 140;

volatile int t = 0;

// GPSの設定
float latitude;
float longitude;
float altitude;
float speed;
float satellites;
unsigned long Time;

// Arduinojsonの設定
StaticJsonDocument<1024> downPacket_tlm;

// LoRa addressの設定? <=2022/11/18時点では不明瞭
byte localAddress = 0xBB; // address of this device
byte destination = 0xFF;  // where we are sending data to

// millis()の設定
unsigned long prev_SEND, interval_SEND;
// unsigned long interval_SERVO, prev_SERVO;

//割り込みの設定
// int interruptPin_1 = 6;
// int interruptPin_2 = 8;

void setup()
{

    //スイッチピンの設定
    pinMode(6, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(6), L_SW, RISING);

    pinMode(8, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(8), W_SW, RISING);

    // initialize serial communications and wait for port to open:
    Serial.begin(9600);

    if (!LoRa.begin(923E6))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }

    if (!GPS.begin())
    {
        Serial.println("Failed to initialize GPS!");
        while (1)
            ;
    }

    // PWM出力ピンの設定
    //mainservo.attach(5);
    //supplyservo.attach(4);

    prev_SEND = 0;
    interval_SEND = 1000;

    //　状態確認用LED
    // PinMode(PIN, OUTPUT);
    // PinMode(PIN, OUTPUT);
}

void loop()
{
    // check if there is new GPS data available
    if (GPS.available())
    {
        // read GPS values
        latitude = GPS.latitude();
        longitude = GPS.longitude();
        altitude = GPS.altitude();
        speed = GPS.speed();
        satellites = GPS.satellites();
        Time = GPS.getTime();

        // Create and send LoRa packet
        downlinkFlightData_tlm();
    }

    if (t == 1)
    {
        t = 0;
        mainservo_deg = 140;
        supplyservo_deg = 90;
        // digitalWrite(PIN, HIGH); //OPEN LED
        // digitalWrite(PIN, LOW);  //CLOSE LED
    }

    if (t == 2)
    {
        t = 0;
        mainservo_deg = 0;
        supplyservo_deg = 0;
        // digitalWrite(PIN, LOW);  //OPEN LED
        // digitalWrite(PIN, HIGH); //CLOSE LED
    }
}

/*
void LUNCH_Position()
{
    supplyservo.write(supply_max_deg);
    supplyservo_deg = supplyservo.read();
    delay(1000);
    for (pos = 0; pos >= 120; pos += 1)
    {
        mainservo.write(pos);
        delayMicroseconds(500000);
        mainservo_deg = mainservo.read();
    }


    if (mainservo_deg == 120)
    {
        // mainservo.detach(5);
    }

}
*/

/*
void WAITING_Position()
{
    supplyservo.write(supply_min_deg);
    supplyservo_deg = supplyservo.read();
    delay(1000);
    for (pos = 120; pos <= 0; pos -= 1)
    {
        mainservo.write(pos);
        delayMicroseconds(500000);
        mainservo_deg = mainservo.read();
    }


    if (mainservo_deg == 0)
    {
        // mainservo.detach(5);
    }
}
*/

/*
void LUNCH_Position()
{
    supplyservo.write(supply_max_deg);    // CLOSE
    supplyservo_deg = supplyservo.read(); //現在の角度を取得
    Serial.println(supplyservo.read());

    //供給路と主流路が同時に開いている状態の防止
    // delayMicroseconds(1000000); // 1,000,000us = 1,000,000E-6s = 1s

    if (supplyservo_deg == 60)
    {
        mainservo.write(main_max_deg);    // OPEN
        mainservo_deg = mainservo.read(); //現在の角度を取得
        Serial.println(mainservo.read());
        // digitalWrite(3, HIGH);
        // digitalWrite(Pin, LOW);
        Serial.println("-- Complete Lunch Position --");
        Serial.println();
    }
}
*/

/*
void WAITING_Position()
{
    supplyservo.write(supply_min_deg);    // OPEN
    supplyservo_deg = supplyservo.read(); //現在の角度を取得
    Serial.println(supplyservo.read());

    //供給路と主流路が同時に開いている状態の防止
    // delayMicroseconds(1000000); // 1,000,000us = 1,000,000E-6s = 1s
    for ()

    if (supplyservo_deg == 0)
    {
        mainservo.write(main_min_deg);    // CLOSE
        mainservo_deg = mainservo.read(); //現在の角度を取得
        Serial.println(mainservo.read());
        // digitalWrite(3, LOW);
        // digitalWrite(PIN, HIGH);
        Serial.println("== Complete WAITING Position ==");
        Serial.println();
    }
}
*/

void L_SW()
{
    t = 1;
}

void W_SW()
{
    t = 2;
}

// Safety ArmedからSafeにした際にサーボが駆動するが、これはサーボが0度以外になっている場合に0に戻っている（つまりダンプ可能な状態）
//同時に割り込みが入った場合は、上から順に読まれていくので、LUNCHI_Position => WAITING_Positionの順となる

void downlinkFlightData_tlm()
{
    if (!LoRa.begin(923E6))
        return;

    unsigned long curr_SEND = millis();
    if ((curr_SEND - prev_SEND) >= interval_SEND)
    {
        downPacket_tlm.clear();
        // downPacket_tlm["sensor"] = "gps";
        downPacket_tlm["lat"] = String(latitude, 8);
        downPacket_tlm["lon"] = String(longitude, 8);
        downPacket_tlm["satellites"] = String(satellites, 1);
        downPacket_tlm["epochTime"] = String(Time, 10);
        downPacket_tlm["mainservoDeg"] = String(mainservo_deg, 1);
        downPacket_tlm["supplyservoDeg"] = String(supplyservo_deg, 1);

        LoRa.beginPacket();
        serializeJson(downPacket_tlm, LoRa);
        LoRa.endPacket();

        Serial.println();
        serializeJson(downPacket_tlm, Serial);
        prev_SEND = curr_SEND;
    }
}

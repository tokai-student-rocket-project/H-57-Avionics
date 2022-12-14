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

// サーボの設定
// Servo mainservo;
// Servo supplyservo;
float supplyservo_deg;
float mainservo_deg;
// int supply_min_deg = 0;
// int supply_max_deg = 90;
//  int main_min_deg = 0;
//  int main_max_deg = 140;

// GPSの設定
float latitude;
float longitude;
float altitude;
float speed;
float satellites;
unsigned long Time;

// Arduinojsonの設定
StaticJsonDocument<1024> downPacket_tlm;
StaticJsonDocument<256> servoPacket;

// LoRa addressの設定? <=2022/11/18時点では不明瞭
byte localAddress = 0xBB; // address of this device
byte destination = 0xFF;  // where we are sending data to

// millis()の設定
unsigned long prev_SEND, interval_SEND;

void setup()
{
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

    prev_SEND = 0;
    interval_SEND = 1000;

    // 　状態確認用LED
    //  PinMode(PIN, OUTPUT);
    //  PinMode(PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    // check if there is new GPS data available
    /*
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
    }*/
    GPS.available();
    latitude = GPS.latitude();
    longitude = GPS.longitude();
    altitude = GPS.altitude();
    speed = GPS.speed();
    satellites = GPS.satellites();
    Time = GPS.getTime();

    downlinkFlightData_tlm();

    deserializeJson(servoPacket, Serial1);
    mainservo_deg = servoPacket["mainservoDeg"];
    supplyservo_deg = servoPacket["supplyservoDeg"];
}

void downlinkFlightData_tlm()
{
    /*
    if (!LoRa.begin(923E6))
        return;
        */

    unsigned long curr_SEND = millis();
    if ((curr_SEND - prev_SEND) >= interval_SEND)
    {
        digitalWrite(LED_BUILTIN, LOW);
        downPacket_tlm.clear();
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
        
        digitalWrite(LED_BUILTIN, HIGH);
        prev_SEND = curr_SEND;
    }
}

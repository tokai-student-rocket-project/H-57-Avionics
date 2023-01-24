/*
  機体からGPS情報を地上に送信するArduinoです。
  ソースファイル名：Inside.ino
  作成者：1CES2116 Hiroki Tsutsumi, <氏名の追加>

  H-57号機に搭載予定
*/

#include <LoRa.h>
#include <SPI.h>
#include <Arduino_MKRGPS.h>
#include <ArduinoJson.h>
#include "StreamUtils.h"

// GPSの設定
float latitude;
float longitude;
float altitude;
float speed;
float satellites;
unsigned long Time;

// MKRGPSShiedでは取得不可能
// float variation;
// float course;

// Arduinojsonの設定
StaticJsonDocument<1024> downPacket_tlm;
StaticJsonDocument<512> downPacket_position;

float supplyservo_deg;
float mainservo_deg;

// LoRa addressの設定? <=2022/11/18時点では不明瞭
// byte localAddress = 0xBB; // address of this device
// byte destination = 0xFF;  // where we are sending data to

// millis()の設定
unsigned long prev_SEND, interval_SEND;
unsigned long prev_POSITION, interval_POSITION;

void setup()
{
    // initialize serial communications and wait for port to open:
    Serial.begin(9600);
    // Serial.begin(115200);
    //  while (!Serial)
    //      continue;
    Serial1.begin(9600);

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
    interval_SEND = 100;
    prev_POSITION = 0;
    interval_POSITION = 1200;

    // 状態確認用LED
    //  PinMode(PIN, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    digitalWrite(4, LOW);
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
        digitalWrite(4, HIGH);
    }

    

    if (Serial1.available() > 0)
    {
        StaticJsonDocument<512> servoPacket;
        DeserializationError err = deserializeJson(servoPacket, Serial1);

        if (err == DeserializationError::Ok)
        {
            digitalWrite(5, HIGH);
            mainservo_deg = servoPacket["mainservoDeg"].as<float>();
            supplyservo_deg = servoPacket["supplyservoDeg"].as<float>();
            downlinkServoPosition();
        }
        else
        {
            // Serial.print("deserializeJson() returned ");
            Serial.println(err.c_str());
            digitalWrite(5, LOW);

            while (Serial1.available() > 0)
                Serial1.read();
        }
    }

    // downlinkFlightData_tlm();
}

void downlinkFlightData_tlm()
{

    // if (!LoRa.begin(923E6))
    // return;
    //

    unsigned long curr_SEND = millis();
    if ((curr_SEND - prev_SEND) >= interval_SEND)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        downPacket_tlm.clear();
        downPacket_tlm["lat"] = String(latitude, 8);
        downPacket_tlm["lon"] = String(longitude, 8);
        downPacket_tlm["satellites"] = String(satellites, 1);
        downPacket_tlm["epochTime"] = String(Time, 10);

        LoRa.beginPacket();
        serializeJson(downPacket_tlm, LoRa);
        LoRa.endPacket(true);

        Serial.println();
        serializeJson(downPacket_tlm, Serial);
        digitalWrite(LED_BUILTIN, LOW);
        prev_SEND = curr_SEND;
    }
}

void downlinkServoPosition()
{
    unsigned long curr_POSITION = millis();
    if ((curr_POSITION - prev_POSITION) >= interval_POSITION)
    {
        downPacket_position.clear();
        downPacket_position["mainservoDeg"] = String(mainservo_deg, 1);
        downPacket_position["supplyservoDeg"] = String(supplyservo_deg, 1);

        LoRa.beginPacket();
        serializeJson(downPacket_position, LoRa);
        LoRa.endPacket(true);

        Serial.println();
        serializeJson(downPacket_position, Serial);

        prev_POSITION = curr_POSITION;
    }
}
/*
  Writer?:Hiroki TSUTSUMI(1CES2116)
  Rocket:H-57
  MCU Series:Arduino
  MCU:SAMD21

  GPS,バルブ位置を取得し、送信する機能を持っています。
*/

#include <LoRa.h>
#include <SPI.h>
#include <Arduino_MKRGPS.h>
#include <ArduinoJson.h>
//#include "StreamUtils.h"

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

    if (!LoRa.begin(921.8E6)) //ARIB Unit Channel Number 30 を採用する。
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
<<<<<<< HEAD
<<<<<<< HEAD
    interval_POSITION = 1500;
=======
    interval_POSITION = 1000;
>>>>>>> 33085ccc91ba07cb07be3134e8f1b1eef6d22198
=======
    interval_POSITION = 1200;
>>>>>>> 1c0a7494f0457d85543bff60ddf13c99d63a2a63

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
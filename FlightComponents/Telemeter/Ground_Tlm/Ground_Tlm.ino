/*
  Writer?:Hiroki TSUTSUMI(1CES2116)
  Rocket:H-57
  MCU Series:Arduino
  MCU:SAMD21

  GPS,バルブ位置を取得し、送信する機能を持っています。
*/

#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

StaticJsonDocument<1024> packet; //課題 > packetのサイズの最適化

void setup()
{
  Serial.begin(115200);
  LoRa.begin(921.8E6); //ARIB Unit Channel Number 30 を採用する。
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{

  if (Serial.available() > 0)
  {
    LoRa.beginPacket();
    LoRa.print(Serial.readStringUntil('\n'));
    LoRa.endPacket();
  }

  if (LoRa.parsePacket())
  {
    deserializeJson(packet, LoRa);
    char output[1024];
    serializeJson(packet, output);
    
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(output);
    
    /*
    Groundの状況に応じて実装

    Serial.print(" || RSSI: "); // 通信強度がわかります。
    Serial.println(LoRa.packetRssi());
    Serial.println();
    */
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
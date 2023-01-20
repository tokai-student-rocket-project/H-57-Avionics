/*
  地上でGPSを受信するArduinoです。
*/

#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

StaticJsonDocument<1024> packet;

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
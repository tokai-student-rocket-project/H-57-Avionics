#include <SPI.h>
#include <LoRa.h>
#include <MsgPacketizer.h>

void setup() {
  Serial.begin(9600);
  LoRa.begin(923E6);
}

void loop() {
  while(Serial.available()) {
    LoRa.beginPacket();
    LoRa.println(Serial.readStringUntil('\n'));
    LoRa.endPacket();
  }
}
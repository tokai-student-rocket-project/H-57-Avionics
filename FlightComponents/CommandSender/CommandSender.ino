#include <SPI.h>
#include <LoRa.h>
#include <MsgPacketizer.h>

void setup() {
  Serial.begin(9600);
  LoRa.begin(923E6);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    LoRa.beginPacket();
    LoRa.write(0x01);
    LoRa.endPacket();
  }

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    if (LoRa.available() > 0) {
      String result = LoRa.readStringUntil('\n');
      Serial.println(result);
    }
  }
}
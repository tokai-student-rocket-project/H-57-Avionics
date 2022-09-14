#include <SPI.h>
#include <LoRa.h>
#include <MsgPacketizer.h>

void setup() {
  Serial1.begin(9600);
  LoRa.begin(920E6);
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      String command = LoRa.readStringUntil('\n');
      MsgPacketizer::send(Serial1, 0x01);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}
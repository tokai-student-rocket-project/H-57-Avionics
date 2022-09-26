#include <SPI.h>
#include <LoRa.h>
#include <MsgPacketizer.h>

void setup() {
  Serial1.begin(9600);
  LoRa.begin(923E6);
}

void loop() {
  // 無線からFMC
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    if (LoRa.available() > 0) {
      Serial1.write(LoRa.read());
      // digitalWrite(LED_BUILTIN, HIGH);
      // delay(200);
      // digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // FMCから無線
  if (Serial1.available() > 0) {
    LoRa.beginPacket();
    LoRa.write(Serial1.read());
    LoRa.endPacket();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
  }
}
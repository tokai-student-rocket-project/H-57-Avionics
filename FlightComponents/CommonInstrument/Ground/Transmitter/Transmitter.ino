#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>


StaticJsonDocument<512> packet;


void setup() {
  Serial.begin(115200);
  LoRa.begin(920E6);
}


void loop() {
  if (Serial.available() > 0) {
    LoRa.beginPacket();
    LoRa.print(Serial.readStringUntil('\n'));
    LoRa.endPacket();
  }

  if (LoRa.parsePacket()) {
    deserializeJson(packet, LoRa);

    char output[256];
    serializeJson(packet, output);
    Serial.println(output);

    Serial.println("{\"t\":\"r\",\"rssi\":\"" + String(LoRa.packetRssi()) + "\"}");
  }
}
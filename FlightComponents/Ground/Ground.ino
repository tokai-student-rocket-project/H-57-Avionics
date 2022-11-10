#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>


StaticJsonDocument<200> packet;


void setup() {
  Serial.begin(9600);
  LoRa.begin(923E6);
}


void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');

    if (command == "0") {
      packet["type"] = "command";
      packet["request"] = "getRefPress";
      sendPacket(packet);
    } else {
      Serial.println("Unknown command. Ignored this operation.");
    }
  }

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String message = LoRa.readStringUntil('\n');
    Serial.println(message);
  }
}

void sendPacket(StaticJsonDocument<200> packet) {
  LoRa.beginPacket();
  serializeJson(packet, LoRa);
  LoRa.endPacket();
}
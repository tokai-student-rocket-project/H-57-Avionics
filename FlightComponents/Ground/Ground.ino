#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(9600);
  LoRa.begin(923E6);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');

    if (command == "INITIALIZE") {
      sendCommand(0x01);
    } else if (command == "HEALTH_CHECK") {
      sendCommand(0x02);
    } else if (command == "SET_REFERENCE_PRESSURE") {
      sendCommand(0x03);
    } else if (command == "SEPARATE") {
      sendCommand(0x04);
    } else if (command == "GET_REFERENCE_PRESSURE") {
      sendCommand(0xF3);
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

void sendCommand(byte command) {
  LoRa.beginPacket();
  LoRa.write(command);
  LoRa.endPacket();
}
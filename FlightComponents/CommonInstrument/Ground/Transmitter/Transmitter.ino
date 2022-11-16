#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>


StaticJsonDocument<4096> packet;


/*
{"type": "req", "req": "getRefPress"}
{"type": "req", "req": "getSepaMin"}
{"type": "req", "req": "getSepaMax"}

{"type": "req", "req": "setRefPress", "v": 1013.0}
{"type": "req", "req": "setSepaMin", "v": 4.0}
{"type": "req", "req": "setSepaMax", "v": 10.0}
*/


void setup() {
  Serial.begin(115200);
  LoRa.begin(923E6);
}


void loop() {
  if (Serial.available() > 0) {
    LoRa.beginPacket();
    LoRa.print(Serial.readStringUntil('\n'));
    LoRa.endPacket();
  }

  if (LoRa.parsePacket()) {
    deserializeJson(packet, LoRa);

    char output[4096];
    serializeJson(packet, output);
    Serial.println(output);
  }
}
#include <SPI.h>
#include <LoRa.h>


/*
{"type": "command", "request": "getRefPress"}
{"type": "command", "request": "getSepaMin"}
{"type": "command", "request": "getSepaMax"}
{"type": "command", "request": "getFlightData"}

{"type": "command", "request": "setRefPress", "value": 1013.0}
{"type": "command", "request": "setSepaMin", "value": 4.0}
{"type": "command", "request": "setSepaMax", "value": 10.0}
*/


void setup() {
  Serial.begin(9600);
  LoRa.begin(923E6);
}


void loop() {
  if (Serial.available() > 0) {
    LoRa.beginPacket();
    LoRa.print(Serial.readStringUntil('\n'));
    LoRa.endPacket();
  }

  if (LoRa.parsePacket()) {
    Serial.println(LoRa.readStringUntil('\n'));
  }
}
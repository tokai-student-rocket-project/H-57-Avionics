#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <MsgPacketizer.h>


StaticJsonDocument<512> packet;


void setup() {
  Serial.begin(115200);
  LoRa.begin(920E6);

  // Status
  MsgPacketizer::subscribe(LoRa, 0x01,
    [](
      uint8_t flightMode,
      bool flightPinState,
      bool shiranui3State,
      bool buzzerState,
      float v33State,
      float v5State,
      float v12State
      )
    {
      packet.clear();
  packet["m"] = String(flightMode);
  packet["f"] = flightPinState ? "1" : "0";
  packet["s3"] = shiranui3State ? "1" : "0";
  packet["b"] = buzzerState ? "1" : "0";
  packet["v33"] = String(v33State, 1);
  packet["v5"] = String(v5State, 1);
  packet["v12"] = String(v12State, 1);
  packet["rssi"] = String(LoRa.packetRssi());
  serializeJson(packet, Serial);
  Serial.println("");
    }
  );

  // FlightData
  MsgPacketizer::subscribe(LoRa, 0x02,
    [](
      float flightTime,
      float altitude,
      float speed,
      float yaw,
      float pitch,
      float roll
      )
    {
      packet.clear();
  packet["ft"] = String(flightTime, 2);
  packet["alt"] = String(altitude, 1);
  packet["s"] = String(speed, 2);
  packet["y"] = String(yaw, 2);
  packet["p"] = String(pitch, 2);
  packet["r"] = String(roll, 2);
  packet["rssi"] = String(LoRa.packetRssi());
  serializeJson(packet, Serial);
  Serial.println("");
    }
  );

  // Config
  MsgPacketizer::subscribe(LoRa, 0x03,
    [](
      float separationAltitude,
      float basePressure,
      float burnTime,
      float separationProtectionTime,
      float forceSeparationTime,
      float landingTime
      )
    {
      packet.clear();
  packet["a"] = String(separationAltitude, 1);
  packet["p"] = String(basePressure, 1);
  packet["bt"] = String(burnTime, 2);
  packet["sp"] = String(separationProtectionTime, 2);
  packet["fs"] = String(forceSeparationTime, 2);
  packet["l"] = String(landingTime, 2);
  packet["rssi"] = String(LoRa.packetRssi());
  serializeJson(packet, Serial);
  Serial.println("");
    }
  );

  // Event
  MsgPacketizer::subscribe(LoRa, 0x04,
    [](
      String event
      )
    {
      packet.clear();
  packet["e"] = event;
  serializeJson(packet, Serial);
  Serial.println("");
    }
  );
}


void loop() {
  if (LoRa.parsePacket()) {
    MsgPacketizer::parse();
  }

  if (Serial.available() > 0) {
    LoRa.beginPacket();
    LoRa.print(Serial.readStringUntil('\n'));
    LoRa.endPacket();
  }
}
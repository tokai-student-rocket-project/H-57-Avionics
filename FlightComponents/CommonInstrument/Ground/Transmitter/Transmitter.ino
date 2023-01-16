#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <MsgPacketizer.h>


StaticJsonDocument<512> downlinkPacket;
StaticJsonDocument<512> upPacket;


void setup() {
  Serial.begin(115200);
  LoRa.begin(923.8E6);

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
      downlinkPacket["m"] = String(flightMode);
  downlinkPacket["f"] = flightPinState ? "1" : "0";
  downlinkPacket["s3"] = shiranui3State ? "1" : "0";
  downlinkPacket["b"] = buzzerState ? "1" : "0";
  downlinkPacket["v33"] = String(v33State, 1);
  downlinkPacket["v5"] = String(v5State, 1);
  downlinkPacket["v12"] = String(v12State, 1);
  downlinkPacket["rssi"] = String(LoRa.packetRssi());
  sendDownlinkPacket();
    }
  );

  // FlightData
  MsgPacketizer::subscribe(LoRa, 0x02,
    [](
      float flightTime,
      float altitude,
      float acceleration,
      float yaw,
      float pitch,
      float roll
      )
    {
      downlinkPacket["ft"] = String(flightTime, 2);
  downlinkPacket["alt"] = String(altitude, 1);
  downlinkPacket["ac"] = String(acceleration, 2);
  downlinkPacket["y"] = String(yaw, 2);
  downlinkPacket["p"] = String(pitch, 2);
  downlinkPacket["r"] = String(roll, 2);
  downlinkPacket["rssi"] = String(LoRa.packetRssi());
  sendDownlinkPacket();
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
      downlinkPacket["a"] = String(separationAltitude, 1);
  downlinkPacket["p"] = String(basePressure, 1);
  downlinkPacket["bt"] = String(burnTime, 2);
  downlinkPacket["sp"] = String(separationProtectionTime, 2);
  downlinkPacket["fs"] = String(forceSeparationTime, 2);
  downlinkPacket["l"] = String(landingTime, 2);
  downlinkPacket["rssi"] = String(LoRa.packetRssi());
  sendDownlinkPacket();
    }
  );

  // Event
  MsgPacketizer::subscribe(LoRa, 0x04,
    [](
      uint32_t id,
      float flightTime,
      String event
      )
    {
      downlinkPacket["id"] = id;
  downlinkPacket["eft"] = flightTime;
  downlinkPacket["e"] = event;
  sendDownlinkPacket();
    }
  );
}


void loop() {
  if (LoRa.parsePacket()) {
    MsgPacketizer::parse();
  }


  int available = Serial.available();
  if (available) {
    deserializeJson(upPacket, Serial);

    if (upPacket["t"] != "c") return;

    float payload = upPacket["v"].as<float>();

    // 指定分離高度
    if (upPacket["l"] == "a") sendCommand(0x00, payload);
    // 基準気圧
    if (upPacket["l"] == "p") sendCommand(0x01, payload);
    // 想定燃焼時間
    if (upPacket["l"] == "bt") sendCommand(0x02, payload);
    // 分離保護時間
    if (upPacket["l"] == "sp") sendCommand(0x03, payload);
    // 強制分離時間
    if (upPacket["l"] == "fs") sendCommand(0x04, payload);
    // 想定着地時間
    if (upPacket["l"] == "l") sendCommand(0x05, payload);

    upPacket.clear();
  }
}


void sendCommand(uint8_t command, float payload) {
  if (LoRa.beginPacket()) {
    MsgPacketizer::send(LoRa, 0xF3, command, payload);
    LoRa.endPacket();
  }
}


void sendDownlinkPacket() {
  serializeJson(downlinkPacket, Serial);
  Serial.println("");
  downlinkPacket.clear();
}
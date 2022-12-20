#include <MsgPacketizer.h>
#include <LoRa.h>
#include <Arduino.h>
#include "Telemeter.h"



void Telemeter::sendPacket(const uint8_t* data, const size_t size) {
  if (LoRa.beginPacket()) {
    LoRa.write(data, size);
    LoRa.endPacket(true);
  }
}


void Telemeter::sendStatus(
  uint8_t flightMode,
  bool flightPinState, bool shiranui3State, bool buzzerState,
  float v33Voltage, float v5Voltage, float v12Voltage
) {
  const auto& packet = MsgPacketizer::encode(
    statusLabel,
    flightMode,
    flightPinState, shiranui3State, buzzerState,
    v33Voltage, v5Voltage, v12Voltage
  );

  sendPacket(packet.data.data(), packet.data.size());
}


void Telemeter::sendFlightData(
  float flightTime,
  float altitude, float speed,
  float yaw, float pitch, float roll
) {
  const auto& packet = MsgPacketizer::encode(
    flightDataLabel,
    flightTime,
    altitude, speed,
    yaw, pitch, roll
  );

  sendPacket(packet.data.data(), packet.data.size());
}


void Telemeter::sendConfig(
  float separationAltitude,
  float basePressure,
  float burnTime,
  float separationProtectionTime,
  float forceSeparationTime,
  float landingTime
) {
  const auto& packet = MsgPacketizer::encode(
    configLabel,
    separationAltitude,
    basePressure,
    burnTime,
    separationProtectionTime,
    forceSeparationTime,
    landingTime
  );

  sendPacket(packet.data.data(), packet.data.size());
}

void Telemeter::sendEvent(
  String event
) {
  const auto& packet = MsgPacketizer::encode(
    eventLabel,
    event
  );

  sendPacket(packet.data.data(), packet.data.size());
}
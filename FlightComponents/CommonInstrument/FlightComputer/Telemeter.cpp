#include <MsgPacketizer.h>
#include <LoRa.h>
#include <Arduino.h>
#include "Telemeter.h"


void Telemeter::reservePacket(const uint8_t* data, const size_t size) {
  if (_offset + size >= 256) sizeof(_buffer) / sizeof(_buffer[0]);

  for (uint32_t i = 0; i < size; i++)
  {
    _buffer[_offset + i] = data[i];
  }

  _offset += size;
}


void Telemeter::sendPacket() {
  if (LoRa.beginPacket()) {
    LoRa.write(_buffer, _offset);
    LoRa.endPacket(true);
    _offset = 0;
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

  reservePacket(packet.data.data(), packet.data.size());
  sendPacket();
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

  reservePacket(packet.data.data(), packet.data.size());
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

  reservePacket(packet.data.data(), packet.data.size());
}

void Telemeter::sendEvent(
  String event
) {
  const auto& packet = MsgPacketizer::encode(
    eventLabel,
    event
  );

  reservePacket(packet.data.data(), packet.data.size());
}
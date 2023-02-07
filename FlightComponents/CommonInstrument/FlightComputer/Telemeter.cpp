#include <MsgPacketizer.h>
#include <LoRa.h>
#include <Arduino.h>
#include "Telemeter.h"


void Telemeter::stackPacket(const uint8_t* data, const size_t size) {
  if (_stackOffset + size > sizeof(_stack) / sizeof(_stack[0])) return;

  for (size_t i = 0; i < size; i++)
  {
    _stack[_stackOffset + i] = data[i];
  }

  _stackOffset += size;
}


void Telemeter::sendStackedData() {
  if (LoRa.beginPacket()) {
    LoRa.write(_stack, _stackOffset);
    LoRa.endPacket(true);
    _stackOffset = 0;
  }
}


void Telemeter::stackStatus(
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

  stackPacket(packet.data.data(), packet.data.size());
}


void Telemeter::stackFlightData(
  float flightTime,
  float altitude, float acceleration,
  float yaw, float pitch, float roll
) {
  const auto& packet = MsgPacketizer::encode(
    flightDataLabel,
    flightTime,
    altitude, acceleration,
    yaw, pitch, roll
  );

  stackPacket(packet.data.data(), packet.data.size());
}


void Telemeter::stackConfig(
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

  stackPacket(packet.data.data(), packet.data.size());
}

void Telemeter::stackEvent(
  uint8_t event,
  float flightTime
) {
  const auto& packet = MsgPacketizer::encode(
    eventLabel,
    _event_count,
    flightTime,
    event
  );

  _event_count++;

  stackPacket(packet.data.data(), packet.data.size());
}
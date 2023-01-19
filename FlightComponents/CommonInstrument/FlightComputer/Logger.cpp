#include <MsgPacketizer.h>
#include <Wire.h>
#include <Arduino.h>
#include "Logger.h"


void Logger::initialize() {
  _packetCount = 0;
}


/// @brief ロガーにフライトデータを書き込む
void Logger::writeLog(
  float flightTime,
  uint8_t flightMode,
  uint8_t StateShiranui3,
  uint8_t StateBuzzer,
  float altitude,
  uint16_t descentCount,
  float accelerationX,
  float accelerationY,
  float accelerationZ,
  float gyroX,
  float gyroY,
  float gyroZ,
  float yaw,
  float pitch,
  float roll
) {
  writeSD(
    flightTime, flightMode,
    StateShiranui3, StateBuzzer,
    altitude,
    descentCount,
    accelerationX, accelerationY, accelerationZ,
    gyroX, gyroY, gyroZ,
    yaw, pitch, roll
  );

  const auto& packet = MsgPacketizer::encode(
    0x12,
    flightTime, flightMode,
    StateShiranui3, StateBuzzer,
    altitude,
    descentCount,
    accelerationX, accelerationY, accelerationZ,
    gyroX, gyroY, gyroZ,
    yaw, pitch, roll
  );

  writeEEPROM(_packetCount, (unsigned char*)packet.data.data(), packet.data.size());

  _packetCount++;
}


void Logger::writeSD(
  float flightTime,
  uint8_t flightMode,
  uint8_t StateShiranui3,
  uint8_t StateBuzzer,
  float altitude,
  uint16_t descentCount,
  float accelerationX,
  float accelerationY,
  float accelerationZ,
  float gyroX,
  float gyroY,
  float gyroZ,
  float yaw,
  float pitch,
  float roll) {
  if (_packetCount == 0) {
    Serial.println("flightTime,flightMode,StateShiranui3,StateBuzzer,altitude,descentCount,accelerationX,accelerationY,accelerationZ,gyroX,gyroY,gyroZ,yaw,pitch,roll");
  }

  Serial.print(flightTime); Serial.print(",");
  Serial.print(flightMode); Serial.print(",");
  Serial.print(StateShiranui3); Serial.print(",");
  Serial.print(StateBuzzer); Serial.print(",");
  Serial.print(altitude); Serial.print(",");
  Serial.print(descentCount); Serial.print(",");
  Serial.print(accelerationX); Serial.print(",");
  Serial.print(accelerationY); Serial.print(",");
  Serial.print(accelerationZ); Serial.print(",");
  Serial.print(gyroX); Serial.print(",");
  Serial.print(gyroY); Serial.print(",");
  Serial.print(gyroZ); Serial.print(",");
  Serial.print(yaw); Serial.print(",");
  Serial.print(pitch); Serial.print(",");
  Serial.print(roll);
  Serial.println("");
}


void Logger::writeEEPROM(const size_t packetCount, const uint8_t* data, const size_t size) {
  size_t address = packetCount * _PAGE_SIZE;
  uint8_t blockAddress = (uint8_t)(address >> 16);
  uint16_t innerAddress = (uint16_t)(address & 0x0000FFFF);

  Wire.beginTransmission(_addresses[blockAddress]);
  Wire.write(highByte(innerAddress));
  Wire.write(lowByte(innerAddress));
  Wire.write(data, size);
  Wire.endTransmission();
}


void Logger::dumpLog() {
  MsgPacketizer::subscribe_manual(0x12,
    [](
      float flightTime,
      uint8_t flightMode,
      uint8_t  StateShiranui3,
      uint8_t  StateBuzzer,
      float altitude,
      uint16_t descentCount,
      float accelerationX,
      float accelerationY,
      float accelerationZ,
      float gyroX,
      float gyroY,
      float gyroZ,
      float yaw,
      float pitch,
      float roll
      ) {
        Serial.print(flightTime); Serial.print(",");
  Serial.print(flightMode); Serial.print(",");
  Serial.print(StateShiranui3); Serial.print(",");
  Serial.print(StateBuzzer); Serial.print(",");
  Serial.print(altitude); Serial.print(",");
  Serial.print(descentCount); Serial.print(",");
  Serial.print(accelerationX); Serial.print(",");
  Serial.print(accelerationY); Serial.print(",");
  Serial.print(accelerationZ); Serial.print(",");
  Serial.print(gyroX); Serial.print(",");
  Serial.print(gyroY); Serial.print(",");
  Serial.print(gyroZ); Serial.print(",");
  Serial.print(yaw); Serial.print(",");
  Serial.print(pitch); Serial.print(",");
  Serial.print(roll);
  Serial.println("");
    }
  );

  uint8_t packet[_PACKET_SIZE];

  Serial.println("flightTime,flightMode,StateShiranui3,StateBuzzer,altitude,descentCount,accelerationX,accelerationY,accelerationZ,gyroX,gyroY,gyroZ,yaw,pitch,roll");

  for (size_t address = 0; address < (size_t)0x7FFFF; address += _PAGE_SIZE) {
    uint8_t blockAddress = (uint8_t)(address >> 16);
    uint16_t innerAddress = (uint16_t)(address & 0x0000FFFF);

    Wire.beginTransmission(_addresses[blockAddress]);
    Wire.write(highByte(innerAddress));
    Wire.write(lowByte(innerAddress));
    Wire.endTransmission();
    Wire.requestFrom(_addresses[blockAddress], _PACKET_SIZE);

    for (int32_t offset = 0; offset < _PACKET_SIZE; offset++) {
      packet[offset] = Wire.read();
    }

    MsgPacketizer::feed(packet, _PACKET_SIZE);
  }
}
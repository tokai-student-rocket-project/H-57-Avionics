#include <MsgPacketizer.h>
#include <Wire.h>
#include <Arduino.h>
#include "Logger.h"


/// @brief ロガーにフライトデータを書き込む
void Logger::writeLog(
  float flightTime,
  uint8_t flightMode,
  uint8_t StateShiranui3,
  uint8_t StateBuzzer,
  float altitude,
  float speed,
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
  const auto& packet = MsgPacketizer::encode(
    0x12,
    flightTime, flightMode,
    StateShiranui3, StateBuzzer,
    altitude, speed,
    descentCount,
    accelerationX, accelerationY, accelerationZ,
    gyroX, gyroY, gyroZ,
    yaw, pitch, roll
  );

  Logger::write(_packetCount, (unsigned char*)packet.data.data(), packet.data.size());

  _packetCount++;
}


void Logger::write(const size_t packetCount, const uint8_t* data, const size_t size) {
  size_t address = packetCount * Logger::_PAGE_SIZE;
  uint8_t blockAddress = (uint8_t)(address >> 16);
  uint16_t innerAddress = (uint16_t)(address & 0x0000FFFF);

  Wire.beginTransmission(_addresses[blockAddress]);
  Wire.write(highByte(innerAddress));
  Wire.write(lowByte(innerAddress));
  Wire.write(data, size);
  Wire.endTransmission();
}


void Logger::initialize() {
  _packetCount = 0;
}
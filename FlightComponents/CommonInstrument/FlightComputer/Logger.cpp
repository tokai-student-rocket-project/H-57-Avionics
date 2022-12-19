
#include <Wire.h>
#include <Arduino.h>
#include "Logger.h"


/// @brief パケットを生成する
void Logger::generatePacket(
  char packet[],
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
  sprintf(packet, "%.2f,%d,%d,%d,%.2f,%.2f,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
    flightTime, flightMode,
    StateShiranui3, StateBuzzer,
    altitude, speed,
    descentCount,
    accelerationX, accelerationY, accelerationZ,
    gyroX, gyroY, gyroZ,
    yaw, pitch, roll);
}


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
  char packet[128];
  Logger::generatePacket(
    packet,
    flightTime, flightMode,
    StateShiranui3, StateBuzzer,
    altitude, speed,
    descentCount,
    accelerationX, accelerationY, accelerationZ,
    gyroX, gyroY, gyroZ,
    yaw, pitch, roll
  );

  int16_t packetSize = sizeof(packet) / sizeof(packet[0]);

  Logger::write(_offset, packet);
  Serial.print(packet);

  _offset += packetSize;
}


void Logger::write(int32_t address, char b[]) {
  uint8_t blockAddress = (uint8_t)(address >> 16);
  uint16_t innerAddress = (uint16_t)(address & 0x0000FFFF);

  Wire.beginTransmission(_addresses[blockAddress]);
  Wire.write(highByte(innerAddress));
  Wire.write(lowByte(innerAddress));
  Wire.write(&b[0], 128);
  Wire.endTransmission();
}
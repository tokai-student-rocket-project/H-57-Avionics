
#include <Wire.h>
#include <Arduino.h>
#include <EEPROM_24xx1025.h>
#include "Logger.h"


#define BUFFER_LENGTH 32
#define EEPROM1025_WRITE_BUFF_SIZE 30
#define EEPROM1025_READ_BUFF_SIZE 32


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
  char packet[256];
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

  for (int16_t packetOffset = 0; packetOffset < packetSize; packetOffset++)
  {
    _eeprom->write(_offset + packetOffset, packet[packetOffset]);
  }

  _offset += packetSize;
}
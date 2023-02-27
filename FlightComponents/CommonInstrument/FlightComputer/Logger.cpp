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
  float pressure,
  float temperature,
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
  float roll,
  float voltage33,
  float voltage5,
  float voltage12
) {
  writeSD(
    flightTime, flightMode,
    StateShiranui3, StateBuzzer,
    pressure, temperature, altitude, descentCount,
    accelerationX, accelerationY, accelerationZ,
    gyroX, gyroY, gyroZ,
    yaw, pitch, roll,
    voltage33, voltage5, voltage12
  );

  const auto& packet = MsgPacketizer::encode(
    0x12,
    flightTime, flightMode,
    StateShiranui3, StateBuzzer,
    pressure, temperature, altitude, descentCount,
    accelerationX, accelerationY, accelerationZ,
    gyroX, gyroY, gyroZ,
    yaw, pitch, roll,
    voltage33, voltage5, voltage12
  );

  writeEEPROM(_packetCount, (unsigned char*)packet.data.data(), packet.data.size());

  _packetCount++;
}


void Logger::writeSD(
  float flightTime,
  uint8_t flightMode,
  uint8_t stateShiranui3,
  uint8_t stateBuzzer,
  float pressure,
  float temperature,
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
  float roll,
  float voltage33,
  float voltage5,
  float voltage12
) {
  if (_packetCount == 0) {
    Serial1.println("flightTime,flightMode,stateShiranui3,stateBuzzer,pressure,temperature,altitude,descentCount,accelerationX,accelerationY,accelerationZ,gyroX,gyroY,gyroZ,yaw,pitch,roll,voltage33,voltage5,voltage12");
  }

  Serial1.print(flightTime); Serial1.print(",");
  Serial1.print(flightMode); Serial1.print(",");
  Serial1.print(stateShiranui3); Serial1.print(",");
  Serial1.print(stateBuzzer); Serial1.print(",");
  Serial1.print(pressure); Serial1.print(",");
  Serial1.print(temperature); Serial1.print(",");
  Serial1.print(altitude); Serial1.print(",");
  Serial1.print(descentCount); Serial1.print(",");
  Serial1.print(accelerationX); Serial1.print(",");
  Serial1.print(accelerationY); Serial1.print(",");
  Serial1.print(accelerationZ); Serial1.print(",");
  Serial1.print(gyroX); Serial1.print(",");
  Serial1.print(gyroY); Serial1.print(",");
  Serial1.print(gyroZ); Serial1.print(",");
  Serial1.print(yaw); Serial1.print(",");
  Serial1.print(pitch); Serial1.print(",");
  Serial1.print(roll); Serial1.print(",");
  Serial1.print(voltage33); Serial1.print(",");
  Serial1.print(voltage5); Serial1.print(",");
  Serial1.print(voltage12);
  Serial1.println("");
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
      uint8_t  stateShiranui3,
      uint8_t  stateBuzzer,
      float pressure,
      float temperature,
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
      float roll,
      float voltage33,
      float voltage5,
      float voltage12
      ) {
        Serial.print(flightTime); Serial.print(",");
        Serial.print(flightMode); Serial.print(",");
        Serial.print(stateShiranui3); Serial.print(",");
        Serial.print(stateBuzzer); Serial.print(",");
        Serial.print(pressure); Serial.print(",");
        Serial.print(temperature); Serial.print(",");
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
        Serial.print(roll); Serial.print(",");
        Serial.print(voltage33); Serial.print(",");
        Serial.print(voltage5); Serial.print(",");
        Serial.print(voltage12);
        Serial.println("");
    }
  );

  uint8_t packet[_PACKET_SIZE];

  Serial.println("flightTime,flightMode,stateShiranui3,stateBuzzer,pressure,temperature,altitude,descentCount,accelerationX,accelerationY,accelerationZ,gyroX,gyroY,gyroZ,yaw,pitch,roll,voltage33,voltage5,voltage12");

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
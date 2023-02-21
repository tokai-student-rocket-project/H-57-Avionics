#include <Wire.h>
#include <Arduino.h>
#include <EEPROM_24xx1025.h>
#include <MsgPacketizer.h>


EEPROM_24xx1025 eeprom1025(EPR_ADDR0, EPR_ADDR1, EPR_ADDR2, EPR_ADDR3);


void setup() {
  Serial.begin(115200);
  while (!Serial);

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
}


constexpr size_t PACKET_SIZE = 88;
constexpr size_t PAGE_SIZE = 128;
int32_t address = 0;
uint8_t packet[88];


void loop() {
  Serial.println("flightTime,flightMode,stateShiranui3,stateBuzzer,pressure,temperature,altitude,descentCount,accelerationX,accelerationY,accelerationZ,gyroX,gyroY,gyroZ,yaw,pitch,roll,voltage33,voltage5,voltage12");

  if (address < eeprom1025.maxLongAddress()) {

    eeprom1025.read(address, packet, PACKET_SIZE);
    MsgPacketizer::feed(packet, PACKET_SIZE);

    address += PAGE_SIZE;
  }
}
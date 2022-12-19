#include <Wire.h>


/// @brief EEPROMロガーの抽象化クラス。https://github.com/citriena/EEPROM_24xx1025 からの移植
class Logger
{
private:

  uint8_t _addresses[8] = { 0x50, 0x54, 0x51, 0x55, 0x52, 0x56, 0x53, 0x57 };
  int32_t _offset;

  void write(int32_t address, char b[]);

public:
  void generatePacket(
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
  );

  /// @brief ロガーにフライトデータを書き込む
  void writeLog(
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
  );
};
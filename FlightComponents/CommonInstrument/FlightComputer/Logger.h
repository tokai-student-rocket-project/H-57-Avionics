#include <Wire.h>


/// @brief EEPROMロガーの抽象化クラス。https://github.com/citriena/EEPROM_24xx1025 からの移植
class Logger
{
private:
  const size_t _PAGE_SIZE = 128;

  uint8_t _addresses[8] = { 0x50, 0x54, 0x51, 0x55, 0x52, 0x56, 0x53, 0x57 };
  size_t _packetCount = 0;

  void write(const size_t packetCount, const uint8_t* data, const size_t size);

public:

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

  void initialize();
};
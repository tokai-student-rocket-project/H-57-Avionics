#include <EEPROM_24xx1025.h>
#include <Wire.h>

#define BUFFER_LENGTH 32
#define EEPROM1025_WRITE_BUFF_SIZE 30
#define EEPROM1025_READ_BUFF_SIZE 32

/// @brief EEPROMロガーの抽象化クラス
class Logger
{
private:
  EEPROM_24xx1025* _eeprom = new EEPROM_24xx1025(EPR_ADDR0, EPR_ADDR1, EPR_ADDR2, EPR_ADDR3);

  int32_t _offset;

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
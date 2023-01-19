#ifndef OUTPUT_PIN_H_
#define OUTPUT_PIN_H_

/// @brief アウトプット設定のピン
class OutputPin
{
private:
  uint8_t _pinNumber;


public:
  /// @brief コンストラクタ
  /// @param pinNumber 設定するピン番号
  OutputPin(uint8_t pinNumber);

  bool isOn();
  void set(bool isOn);

  void on();
  void off();
};

#endif
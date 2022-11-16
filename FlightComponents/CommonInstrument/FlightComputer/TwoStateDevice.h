#ifndef TWO_STATE_DEVICE_H_
#define TWO_STATE_DEVICE_H_

/// @brief オン/オフの抽象化クラス
class TwoStateDevice
{
private:
  int _pinNumber;


public:
  /// @brief コンストラクタ
  /// @param pinNumber Arduinoでのピン番号
  TwoStateDevice(int pinNumber);

  /// @brief 初期化
  void initialize();

  void on();

  void off();

  void setState(bool state);

  bool getState();
};

#endif
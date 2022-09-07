#ifndef LED_H_
#define LED_H_

/// @brief LEDの抽象化クラス
class LED
{
public:
  /// @brief コンストラクタ
  /// @param pin Arduinoのデジタルピン
  LED(int pin);

  /// @brief LEDを初期化する
  void initialize();

  /// @brief LEDを点灯させる
  void on();

  /// @brief LEDを消灯させる
  void off();
  
private:
  int _pin;
};

#endif
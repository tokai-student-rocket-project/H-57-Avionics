#ifndef FLIGHT_PIN_H_
#define FLIGHT_PIN_H_

/// @brief フライトピンの抽象化クラス
class FlightPin
{
public:
  /// @brief コンストラクタ
  /// @param pin Arduinoのデジタルピン
  FlightPin(int pin);

  /// @brief フライトピンを初期化する
  void initialize();

  /// @brief フライトピンが抜けているかを返す
  /// @return 抜けていればtrue
  bool isReleased();
  
private:
  int _pin;
};

#endif
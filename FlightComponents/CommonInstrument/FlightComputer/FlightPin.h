#ifndef FLIGHT_PIN_H_
#define FLIGHT_PIN_H_

/// @brief フライトピンの抽象化クラス
class FlightPin
{
private:
  /// @brief チャタリング対策
  const uint16_t MINIMUM_COUNT = 10;

  uint8_t _pinNumber;
  uint16_t _releaseCount;
  uint16_t _restraintCount;
  bool _isReleased;


public:
  /// @brief コンストラクタ
  /// @param pinNumber フライトピンのArduinoでのピン番号
  FlightPin(uint8_t pinNumber);

  /// @brief フライトピンを初期化
  void initialize();

  /// @brief フライトピンの状態を更新
  void update();

  /// @brief リリースされたかを返す
  bool isReleased();
};

#endif
#ifndef FLIGHT_PIN_H_
#define FLIGHT_PIN_H_

/// @brief フライトピンの抽象化クラス
class FlightPin
{
private:
  /// @brief チャタリング対策
  const int MINIMUM_COUNT = 10;

  int _pinNumber;
  int _releaseCount;
  int _restraintCount;
  bool _isReleased;


public:
  /// @brief コンストラクタ
  /// @param pinNumber フライトピンのArduinoでのピン番号
  FlightPin(int pinNumber);

  /// @brief フライトピンを初期化
  void initialize();

  /// @brief フライトピンの状態を更新
  void update();

  /// @brief リリースされたかを返す
  bool isReleased();
};

#endif
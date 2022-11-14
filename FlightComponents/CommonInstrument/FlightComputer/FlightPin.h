#ifndef FLIGHT_PIN_H_
#define FLIGHT_PIN_H_

/// @brief フライトピンの抽象化クラス
class FlightPin
{
private:
  /// @brief 回数で指定。この値以上に連続でリリースを検知しないと飛行中にならない
  const int MINIMUM_RELEASE_COUNT = 10;

  int _pinNumber;
  int _releaseCount;
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
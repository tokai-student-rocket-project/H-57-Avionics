#ifndef UPLLUP_PIN_H_
#define UPLLUP_PIN_H_

/// @brief プルアップ設定のピン
class PullupPin
{
private:
  /// @brief チャタリング対策
  const uint16_t MINIMUM_COUNT = 10;

  uint8_t _pinNumber;
  uint16_t _openCount;
  uint16_t _closeCount;
  bool _isOpen;


public:
  /// @brief コンストラクタ
  /// @param pinNumber 設定するピン番号
  PullupPin(uint8_t pinNumber);

  /// @brief ピンの状態を更新
  void update();

  /// @brief 状態を返す
  bool isOpen();
};

#endif
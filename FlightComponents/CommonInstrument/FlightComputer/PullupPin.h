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

  bool _initialState;


public:
  /// @brief コンストラクタ
  /// @param pinNumber 設定するピン番号
  PullupPin(uint8_t pinNumber);

  /// @brief ピンの状態を更新
  void update();

  /// @brief チャタリング対策済みの現在の状態を返す
  bool isOpen();

  /// @brief 初期化時から開になっていたかを返す
  bool isOpenWhenInitializing();

  /// @brief チャタリング対策をしていない現在の状態を返す
  bool isOpenActually();
};

#endif
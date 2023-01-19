#include <Arduino.h>
#include "PullupPin.h"


/// @brief コンストラクタ
/// @param pinNumber 設定するピン番号
PullupPin::PullupPin(uint8_t pinNumber) {
  _pinNumber = pinNumber;
  pinMode(pinNumber, INPUT_PULLUP);
}


/// @brief ピンの状態を更新
void PullupPin::update() {
  if (digitalRead(_pinNumber) == HIGH) {
    ++_openCount;
    _closeCount = 0;
  }
  else {
    ++_closeCount;
    _openCount = 0;
  }

  if (_openCount >= MINIMUM_COUNT) {
    _isOpen = true;
  }

  if (_closeCount >= MINIMUM_COUNT) {
    _isOpen = false;
  }
}


/// @brief 状態を返す
bool PullupPin::isOpen() {
  return _isOpen;
}
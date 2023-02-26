#include <Arduino.h>
#include "PullupPin.h"


/// @brief コンストラクタ
/// @param pinNumber 設定するピン番号
PullupPin::PullupPin(uint8_t pinNumber) {
  _pinNumber = pinNumber;
  pinMode(pinNumber, INPUT_PULLUP);

  _initialState = isOpenActually();
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


/// @brief チャタリング対策済みの現在の状態を返す
bool PullupPin::isOpen() {
  return _isOpen;
}


/// @brief 初期化時から開になっていたかを返す
bool PullupPin::isOpenWhenInitializing() {
  return _initialState;
}


/// @brief チャタリング対策をしていない現在の状態を返す
bool PullupPin::isOpenActually() {
  return digitalRead(_pinNumber) == HIGH;
}
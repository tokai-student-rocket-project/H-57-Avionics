#include <Arduino.h>
#include "FlightPin.h"


/// @brief コンストラクタ
/// @param pinNumber フライトピンのArduinoでのピン番号
FlightPin::FlightPin(int pinNumber) {
  _pinNumber = pinNumber;
}


/// @brief フライトピンを初期化
void FlightPin::initialize() {
  pinMode(_pinNumber, INPUT_PULLUP);
}


/// @brief フライトピンの状態を更新
void FlightPin::update() {
  bool isReleased = digitalRead(_pinNumber) == HIGH;

  if (isReleased) {
    ++_releaseCount;
  }
  else {
    _releaseCount = 0;
  }

  _isReleased = _releaseCount >= MINIMUM_RELEASE_COUNT;
}


/// @brief リリースされたかを返す
bool FlightPin::isReleased() {
  return _isReleased;
}
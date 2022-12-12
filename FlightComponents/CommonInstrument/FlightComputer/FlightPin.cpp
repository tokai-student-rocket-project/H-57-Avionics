#include <Arduino.h>
#include "FlightPin.h"


/// @brief コンストラクタ
/// @param pinNumber フライトピンのArduinoでのピン番号
FlightPin::FlightPin(uint8_t pinNumber) {
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
    _restraintCount = 0;
  }
  else {
    _releaseCount = 0;
    ++_restraintCount;
  }

  if (_releaseCount >= MINIMUM_COUNT) {
    _isReleased = true;
  }

  if (_restraintCount >= MINIMUM_COUNT) {
    _isReleased = false;
  }
}


/// @brief リリースされたかを返す
bool FlightPin::isReleased() {
  return _isReleased;
}
#include <Arduino.h>
#include "FlightPin.h"

/// @brief コンストラクタ
/// @param pin Arduinoのデジタルピン
FlightPin::FlightPin(int pin) {
  _pin = pin;
}

/// @brief フライトピンを初期化する
void FlightPin::initialize() {
  pinMode(_pin, INPUT_PULLUP);
}

/// @brief フライトピンが抜けているかを返す
/// @return 抜けていればtrue
bool FlightPin::isReleased() {
  return digitalRead(_pin);
}
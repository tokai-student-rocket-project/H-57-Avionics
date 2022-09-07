#include <Arduino.h>
#include "LED.h"

/// @brief コンストラクタ
/// @param pin Arduinoのデジタルピン
LED::LED(int pin) {
  _pin = pin;
}

/// @brief LEDを初期化する
void LED::initialize() {
  pinMode(_pin, OUTPUT);
}

/// @brief LEDを点灯させる
void LED::on() {
  digitalWrite(_pin, HIGH);
}

/// @brief LEDを消灯させる
void LED::off() {
  digitalWrite(_pin, LOW);
}
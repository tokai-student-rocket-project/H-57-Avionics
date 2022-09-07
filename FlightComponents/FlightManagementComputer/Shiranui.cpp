#include <Arduino.h>
#include "Shiranui.h"

/// @brief コンストラクタ
/// @param pin Arduinoのデジタルピン
Shiranui::Shiranui(int pin) {
  _pin = pin;
}

/// @brief 不知火を初期化する
void Shiranui::initialize() {
  pinMode(_pin, OUTPUT);
}

/// @brief 不知火に分離指令を出す
void Shiranui::separate() {
  digitalWrite(_pin, HIGH);
}

/// @brief 不知火の分離指令をリセットします
void Shiranui::reset() {
  digitalWrite(_pin, LOW);
}
#include <Arduino.h>
#include "TwoStateDevice.h"


/// @brief コンストラクタ
/// @param pinNumber Arduinoでのピン番号
TwoStateDevice::TwoStateDevice(int pinNumber) {
  _pinNumber = pinNumber;
}


/// @brief LEDを初期化
void TwoStateDevice::initialize() {
  pinMode(_pinNumber, OUTPUT);
}


void TwoStateDevice::on() {
  digitalWrite(_pinNumber, HIGH);
}


void TwoStateDevice::off() {
  digitalWrite(_pinNumber, LOW);
}


void TwoStateDevice::set(bool state) {
  if (state) {
    on();
  } else {
    off();
  }
}
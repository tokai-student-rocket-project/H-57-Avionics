#include <Arduino.h>
#include "TwoStateDevice.h"


/// @brief コンストラクタ
/// @param pinNumber Arduinoでのピン番号
TwoStateDevice::TwoStateDevice(uint8_t pinNumber) {
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


void TwoStateDevice::setState(bool state) {
  if (state) {
    on();
  }
  else {
    off();
  }
}

bool TwoStateDevice::getState() {
  return digitalRead(_pinNumber);
}
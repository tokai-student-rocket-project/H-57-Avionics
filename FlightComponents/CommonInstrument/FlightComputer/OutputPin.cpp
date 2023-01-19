#include <Arduino.h>
#include "OutputPin.h"


/// @brief コンストラクタ
/// @param pinNumber 設定するピン番号
OutputPin::OutputPin(uint8_t pinNumber) {
  _pinNumber = pinNumber;
  pinMode(pinNumber, OUTPUT);
}


bool OutputPin::isOn() {
  return digitalRead(_pinNumber);
}


void OutputPin::set(bool isOn) {
  digitalWrite(_pinNumber, isOn);
}


void OutputPin::on() {
  set(HIGH);
}


void OutputPin::off() {
  set(LOW);
}

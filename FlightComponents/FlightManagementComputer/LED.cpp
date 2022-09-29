#include <Arduino.h>
#include "LED.h"


LED::LED(int pinNumber) {
  _pinNumber = pinNumber;
  pinMode(_pinNumber, OUTPUT);
}

void LED::set(bool state) {
  if (state) {
    on();
  } else {
    off();
  }
}


void LED::on() {
  digitalWrite(_pinNumber, HIGH);
}


void LED::off() {
  digitalWrite(_pinNumber, LOW);
}
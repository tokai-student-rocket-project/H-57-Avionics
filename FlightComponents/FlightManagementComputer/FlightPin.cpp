#include <Arduino.h>
#include "FlightPin.h"


FlightPin::FlightPin(uint8_t pinNumer) {
  _pinNumber = pinNumer;
  pinMode(_pinNumber, INPUT_PULLUP);
}


bool FlightPin::isReleased() {
  return digitalRead(_pinNumber);
}
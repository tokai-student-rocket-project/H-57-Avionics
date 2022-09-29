#include <Arduino.h>
#include "PressureSensor.h"


void PressureSensor::initialize() {
  _bme280.beginI2C();
}


float PressureSensor::getPressure() {
  return _bme280.readFloatPressure();
}


void PressureSensor::setReferencePressure(float referencePressure) {
  _bme280.setReferencePressure(referencePressure);
}

float PressureSensor::getReferencePressure() {
  _bme280.getReferencePressure();
}


float PressureSensor::getAltitude() {
  return _bme280.readFloatAltitudeMeters();
}
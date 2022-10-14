#include <Arduino.h>
#include "BME280Wrap.h"


void BME280Wrap::initialize() {
  _bme280.beginI2C();
}


float BME280Wrap::getPressure() {
  return _bme280.readFloatPressure();
}


void BME280Wrap::setReferencePressure(float referencePressure) {
  _bme280.setReferencePressure(referencePressure);
}

float BME280Wrap::getReferencePressure() {
  _bme280.getReferencePressure();
}


float BME280Wrap::getAltitude() {
  return _bme280.readFloatAltitudeMeters();
}

float BME280Wrap::getHumidity() {
  return _bme280.readFloatHumidity();
}

float BME280Wrap::getTemperature() {
  return _bme280.readTempC();
}
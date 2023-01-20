#include <Arduino.h>
#include "Altimeter.h"


void Altimeter::initialize() {
  _bme280.beginI2C();

  // 時間を空けないと正常な気圧を取得できない
  delay(5000);
  setReferencePressure(getPressure());
}


float Altimeter::getPressure() {
  // SparkFunBME280の仕様でreadFloatPressureの前にreadTempCを呼び出さないと正確な気圧を得られない。
  getTemperature();
  return _bme280.readFloatPressure();
}


void Altimeter::setReferencePressure(float referencePressure) {
  _bme280.setReferencePressure(referencePressure);
}


float Altimeter::getReferencePressure() {
  _bme280.getReferencePressure();
}


float Altimeter::getAltitude() {
  float altitude = _bme280.readFloatAltitudeMeters();

  _descentDetector.updateAltitude(altitude);
  return altitude;
}


float Altimeter::getTemperature() {
  return _bme280.readTempC();
}


bool Altimeter::isDescending() {
  return _descentDetector._isDescending;
}
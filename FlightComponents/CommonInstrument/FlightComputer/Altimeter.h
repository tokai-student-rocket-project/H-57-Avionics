#ifndef ALTIMETER_H_
#define ALTIMETER_H_


#include <SparkFunBME280.h>
#include "DescentDetector.h"


class Altimeter {
private:
  BME280 _bme280;

  // 降下検知用
  DescentDetector _descentDetector;

public:
  void initialize();

  float getPressure();
  float getAltitude();
  float getTemperature();

  void setReferencePressure(float referencePressure);
  float getReferencePressure();

  bool isDescending();
};


#endif
#ifndef PRESSURE_SENSOR_H_
#define PRESSURE_SENSOR_H_


#include <SparkFunBME280.h>


class PressureSensor {
private:
  BME280 _bme280;

public:
  void initialize();

  float getPressure();

  void setReferencePressure(float referencePressure);
  float getReferencePressure();
  float getAltitude();
};


#endif
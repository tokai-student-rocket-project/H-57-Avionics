#ifndef BME280_Wrap_H_
#define BME280_Wrap_H_


#include <SparkFunBME280.h>


class BME280Wrap {
private:
  BME280 _bme280;

public:
  void initialize();

  float getPressure();
  float getAltitude();
  float getHumidity();
  float getTemperature();

  void setReferencePressure(float referencePressure);
  float getReferencePressure();
};


#endif
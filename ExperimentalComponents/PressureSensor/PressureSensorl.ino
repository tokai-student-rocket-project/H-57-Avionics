#include <Wire.h>
#include "LPS22HB.h"

// センサのインスタンス
LPS22HB PressureSensor;

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  PressureSensor.initialize();
}

void loop()
{
  Serial.println(PressureSensor.getTemperature());
}

#include <Arduino.h>
#include "FrequencyTimer.h"


void FrequencyTimer::delay() {
  unsigned int processTime_us = micros() - _originTime_us;
  unsigned int trueDelayTime_us = _delayTime_us - processTime_us;

  if (trueDelayTime_us <= _delayTime_us) delayMicroseconds(trueDelayTime_us);

  _originTime_us = micros();
}
#include <Arduino.h>
#include "DescentDetector.h"

DescentDetector::DescentDetector() {
}

void DescentDetector::update(double altitude)
{
  float average = altitude_average_.getAverage(altitude);

  if ((altitude_average_old_ - average) > DESCENT_DIFFERENT_) {
    ++descent_count_;
  }
  else {
    descent_count_ = 0;
  }

  altitude_average_old_ = average;
  is_descending_ = descent_count_ >= MINIMUM_DESCENT_COUNT_;

  Serial.print(altitude);
  Serial.print("\t");
  Serial.print(average);
  Serial.print("\t");
  Serial.println(descent_count_);
}

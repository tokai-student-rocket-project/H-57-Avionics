#include <Arduino.h>
#include "DescentDetector.h"

DescentDetector::DescentDetector(double weight) {
  weight_ = weight;
}

void DescentDetector::update(double altitude)
{
  average_ = average_old_ + weight_ * (altitude - average_old_);

  if ((average_old_ - average_) > DESCENT_DIFFERENT_) {
    ++descent_count_;
  }
  else {
    descent_count_ = 0;
  }

  average_old_ = average_;
  is_descending_ = descent_count_ >= MINIMUM_DESCENT_COUNT_;

  Serial.print(altitude);
  Serial.print("\t");
  Serial.print(average_);
  Serial.print("\t");
  Serial.println(descent_count_);
}

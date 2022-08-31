#include <Arduino.h>
#include "MovingAverage.h"

// weight: 1に近いほど弱い平滑化、0に近いほど強い平滑化
MovingAverage::MovingAverage(double weight) {
  weight_ = weight;
}

double MovingAverage::getAverage(double value)
{
  double average = average_old_ + weight_ * (value - average_old_);
  average_old_ = average;
  return average;
}

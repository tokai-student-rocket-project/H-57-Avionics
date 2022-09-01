#ifndef DESCENT_DETECTOR_H_
#define DESCENT_DETECTOR_H_

#include "MovingAverage.h"

class DescentDetector
{
public:
  DescentDetector();

  void update(double altitude);

  bool is_descending_;
  
private:
  const double DESCENT_DIFFERENT_ = 0.0;
  const int MINIMUM_DESCENT_COUNT_ = 10;

  MovingAverage altitude_average_ = MovingAverage(0.2);

  double altitude_average_old_;
  int descent_count_;
};

#endif
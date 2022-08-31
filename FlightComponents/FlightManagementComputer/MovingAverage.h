#ifndef MOVING_AVERAGE_H_
#define MOVING_AVERAGE_H_

// 指数平滑化移動平均
class MovingAverage
{
public:
  MovingAverage(double weight);

  double getAverage(double value);

private:
  double weight_;
  double average_old_;
};

#endif
#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

class MovingAverage
{
public:
  MovingAverage();

  void addData(float data);
  float getAverage();

private:
  void moveCurrent();

  float buffer[5];
  int current = 0;
};

#endif
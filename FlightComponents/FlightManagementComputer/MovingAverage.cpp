#include <Arduino.h>
#include "MovingAverage.h"

MovingAverage::MovingAverage()
{
}

void MovingAverage::moveCurrent()
{
  current++;

  if (current >= 5)
  {
    current = 0;
  }
}

void MovingAverage::addData(float data)
{
  buffer[current] = data;
  moveCurrent();
}

float MovingAverage::getAverage()
{
  float average;
  for (int i = 0; i < 5; i++)
  {
    average += buffer[i];
  }

  return average / 5.0;
}

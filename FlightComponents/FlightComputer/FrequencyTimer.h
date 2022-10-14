#ifndef FREQUENCY_TIMER_H_
#define FREQUENCY_TIMER_H_


class FrequencyTimer {
private:
  unsigned int _delayTime_us = 10000;
  unsigned int _originTime_us;

public:
  void delay();
};


#endif
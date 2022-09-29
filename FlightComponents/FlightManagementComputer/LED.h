#ifndef LED_H_
#define LED_H_


class LED {
private:
  int _pinNumber;

public:
  LED(int pinNumber);

  void set(bool state);
  void on();
  void off();
};


#endif
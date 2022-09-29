#ifndef FLIGHT_PIN_H_
#define FLIGHT_PIN_H_


class FlightPin {
private:
  uint8_t _pinNumber;

public:
  FlightPin(uint8_t pinNumber);

  bool isReleased();
};


#endif
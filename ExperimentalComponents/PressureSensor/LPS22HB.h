#ifndef LPS22HB_H
#define LPS22HB_H

#define LPS22HB_ADDRESS_AD0_LOW 0x5C
#define LPS22HB_ADDRESS_AD0_HIGH 0x5D
#define LPS22HB_DEFAULT_ADDRESS LPS22HB_ADDRESS_AD0_LOW

#define LPS22HB_CONTROL_REGISTER 0x10
#define LPS22HB_CONTROL_75HZ 0x50

#define LPS22HB_PRESSURE_OUT 0x28
#define LPS22HB_TEMPERATURE_OUT 0x2B

class LPS22HB
{
public:
  LPS22HB(int address = LPS22HB_DEFAULT_ADDRESS);

  void initialize();
  float getPressure();
  float getTemperature();

private:
  int DeviceAddress;
};

#endif
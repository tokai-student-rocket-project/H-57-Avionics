#include <MultiUART.h>
#include <Arduino_JSON.h>

// シリアル通信
// https://github.com/askn37/MultiUART
MultiUART PC(Serial);
MultiUART FDC(2, 3);

// FDCから送られるフライトデータ
JSONVar FlightData;

// フライトデータを構成する数値
float Pressure;
float Altitude;

void setup()
{
  PC.begin(9600);
  FDC.begin(9600);
}

void loop()
{
  if (FDC.available())
  {
    while (FDC.available())
    {
      PC.write(FDC.read());
    }
  }
}
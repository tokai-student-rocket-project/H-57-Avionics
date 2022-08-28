#include <MultiUART.h>
#include <MsgPacketizer.h>

MultiUART FDC(2, 3);

// フライトデータを構成する数値
float Pressure;
float Altitude;

void setup()
{
  Serial.begin(9600);
  FDC.begin(9600);

  Serial.println("Start");

  MsgPacketizer::subscribe(FDC, 0x00, Pressure, Altitude);
}

void loop()
{
  MsgPacketizer::parse();

  Serial.print(Pressure);
  Serial.print("\t");
  Serial.println(Altitude);
}
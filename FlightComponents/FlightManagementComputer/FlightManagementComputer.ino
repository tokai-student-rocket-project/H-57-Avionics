#include <MultiUART.h>
#include <MsgPacketizer.h>

MultiUART DEBUG(Serial);
MultiUART FDC(2, 3);

void setup()
{
  Serial.begin(9600);
  FDC.begin(9600);

  MsgPacketizer::subscribe(FDC, 0x00, [](float altitude)
                           { DEBUG.println(altitude); });
}

void loop()
{
  MsgPacketizer::parse();
}
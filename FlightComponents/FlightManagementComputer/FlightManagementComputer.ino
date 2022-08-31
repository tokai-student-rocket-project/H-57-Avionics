#include <MultiUART.h>
#include <MsgPacketizer.h>
#include "MovingAverage.h"

MultiUART DEBUG(Serial);
MultiUART FDC(2, 3);

MovingAverage PressureAverage;
float PressureAverageOrigin;
int EdgeCount = 0;

void setup()
{
  Serial.begin(9600);
  FDC.begin(9600);

  MsgPacketizer::subscribe(FDC, 0x00, [](float altitude)
                           {
  PressureAverage.addData(altitude);
  float average = PressureAverage.getAverage();

  if (average < PressureAverageOrigin){
    EdgeCount++;
  }
  else{
    EdgeCount = 0;
  }

  PressureAverageOrigin = average;

  if (EdgeCount >= 10){
    DEBUG.println("Apogee");
  } });
}

void loop()
{
  MsgPacketizer::parse();
}
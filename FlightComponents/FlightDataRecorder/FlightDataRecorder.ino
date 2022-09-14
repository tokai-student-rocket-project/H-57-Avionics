#include <SoftwareSerial.h>
#include <MsgPacketizer.h>
#include <MultiUART.h>

MultiUART sdaaaa(2,3);
MultiUART flightManagementComputer(4,5);
MultiUART PC(Serial);

void setup() {
  sdaaaa.begin(9600);
  PC.begin(9600);
  flightManagementComputer.begin(9600);
}
void loop(){
  if (flightManagementComputer.available()) {
  PC.println(flightManagementComputer.read());
}
}

/*
  地上でGPSを受信するArduinoです。
*/

#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

StaticJsonDocument<1024> packet;

void setup()
{
  Serial.begin(115200);
  LoRa.begin(923E6);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{

  if (Serial.available() > 0)
  {
    LoRa.beginPacket();
    LoRa.print(Serial.readStringUntil('\n'));
    LoRa.endPacket();
  }

  if (LoRa.parsePacket())
  {
    deserializeJson(packet, LoRa);
    char output[1024];
    serializeJson(packet, output);
    
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(output);
    
    /*
    Serial.print(" || RSSI: "); // 通信強度がわかります。
    Serial.println(LoRa.packetRssi());
    Serial.println();
    */
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

/*
void onReceive(int packetSize)
{
  if (packetSize == 0)
    return; // if there's no packet, return

  int recipient = LoRa.read();
  String incoming = "";

  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }

  if (recipient != localAddress && recipient != 0xFF)
  {
    Serial.println("This message is not for me.");
    return; // skip rest of function
  }

  Serial.print(incoming);
  Serial.print(" || RSSI: "); //通信強度がわかります。
  Serial.println(LoRa.packetRssi());
  Serial.println();
}
*/

/*
void sendOpen(byte Opencommand) {
  LoRa.beginPacket();
  LoRa.write(Opencommand); //Open
  LoRa.endPacket();
  delay(100);
}
*/

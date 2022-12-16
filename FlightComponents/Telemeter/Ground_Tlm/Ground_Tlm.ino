/*
  地上でGPSを受信するArduinoです。
*/

#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

StaticJsonDocument<1024> packet;
//String message;

byte localAddress = 0xFF;

// const int Switch = 1;

void setup()
{
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  while (!Serial)
    ;
  Serial.println("LoRa GPS data receiver");
  if (!LoRa.begin(923E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  delay(100);
}
void loop()
{

  if (LoRa.parsePacket())
  {
    digitalWrite(LED_BUILTIN, LOW);
    deserializeJson(packet, LoRa);
    char output[1024];
    serializeJson(packet, output);
    Serial.println(output);
    digitalWrite(LED_BUILTIN, HIGH);
  }


  // onReceive(LoRa.parsePacket());

  // int Switch_value;
  // Switch_value = digitalRead(Switch);
  // Serial.println(Switch_value);

  /*
  if (Serial.available() > 0) {
    String Opencommand = Serial.readStringUntil('\n');

    if (Opencommand == "0") {
      sendOpen(0x00); //0x00を送信します。
      Serial.println("Servo was moved CLOSE position.");
    } else if (Opencommand == "1") {
      sendOpen(0x01); //0x01を送信します。
      Serial.println("Servo was moved OPEN position.");
    } else {
      Serial.println("!! Unknown command !!");
    }
  }
  */
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

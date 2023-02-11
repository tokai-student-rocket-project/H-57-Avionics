// LoRaのヘッダーファイル

#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

StaticJsonDocument<1024> packet;

// LoRaのヘッダーファイル

// SSD1316のヘッダーファイル

// #include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); // High speed I2C

// U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);    //Low spped I2C

// SSD1316のヘッダーファイル

void setup()
{
    Serial.begin(9600);
    u8g2.begin();
    Wire.begin();

    LoRa.begin(921.8E6);
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

        // Serial.print(" || RSSI: "); // 通信強度がわかります。
        // Serial.println(LoRa.packetRssi());
        // Serial.println();

        float rssi = LoRa.packetRssi();
        //char str_rssi[10];

        //dtostrf(rssi, 4, 2, str_rssi);

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);

        u8g2.drawStr(0, 10, "RSSI: ");
        //u8g2.drawStr(40, 10, str_rssi);
        u8g2.drawStr(40, 10, rssi);

        u8g2.sendBuffer();
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
}

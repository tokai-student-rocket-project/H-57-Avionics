/*
  機体からGPS情報を地上に送信するArduinoです。
  ソースファイル名：Inside.ino
  作成者：1CES2116 Hiroki Tsutsumi, <氏名の追加>

  H-57号機に搭載予定

*/

#include <LoRa.h>
#include <SPI.h>
#include <Arduino_MKRGPS.h>
#include <Servo.h>
#include <ArduinoJson.h>

//サーボの設定
Servo mainservo;
Servo supplyservo;
volatile int main_deg = 0;
volatile int supply_deg = 0;
float supplyservo_deg;
float mainservo_deg;
// int min_deg = 0;
// int max_deg = 90;

// GPSの設定
float latitude;
float longitude;
float altitude;
float speed;
float satellites;
unsigned long Time;

// Arduinojsonの設定
StaticJsonDocument<1024> downPacket_tlm;

// LoRa addressの設定? <=2022/11/18時点では不明瞭
byte localAddress = 0xBB; // address of this device
byte destination = 0xFF;  // where we are sending data to

// millis()の設定
unsigned long prev_SEND, interval_SEND;
unsigned long interval_SERVO, prev_SERVO;

void setup()
{

    //スイッチピンの設定
    pinMode(6, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(6), LUNCH_Position, RISING);

    pinMode(8, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(8), WAITING_Position, RISING);

    // initialize serial communications and wait for port to open:
    Serial.begin(9600);

    if (!LoRa.begin(923E6))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }

    if (!GPS.begin())
    {
        Serial.println("Failed to initialize GPS!");
        while (1)
            ;
    }

    // PWM出力ピンの設定
    mainservo.attach(5);
    supplyservo.attach(4);

    prev_SEND = 0;
    interval_SEND = 1000;

    prev_SERVO = 0;
    interval_SERVO = 5000;

    pinMode(3, OUTPUT);
}

void loop()
{
    // check if there is new GPS data available

    if (GPS.available())
    {
        // read GPS values
        latitude = GPS.latitude();
        longitude = GPS.longitude();
        altitude = GPS.altitude();
        speed = GPS.speed();
        satellites = GPS.satellites();
        Time = GPS.getTime();

        // Create and send LoRa packet
        downlinkFlightData_tlm();
    }
}


/*
void LUNCH_Position()
{
    unsigned long curr_SERVO;

    for (deg = 0; deg <= 90; deg++)
    {
        supplyservo.write(deg);               // CLOSE
        supplyservo_deg = supplyservo.read(); //現在の角度を取得
    }

    unsigned long curr_SERVO = millis();
    if ((curr_SERVO - prev_SERVO) >= interval_SERVO)
    {
        for (deg = 0; deg <= 90; deg++)
        {
            mainservo.write(deg);             // OPEN
            mainservo_deg = mainservo.read(); //現在の角度を取得
        }

        prev_SERVO = curr_SERVO;
    }
    // digitalWrite(3, HIGH);
    Serial.println("-- Complete Lunch Position --");
    Serial.println();
}
*/

void LUNCH_Position()
{
    for (supply_deg = 0; supply_deg <= 90; supply_deg++)
    {
        supplyservo.write(supply_deg);   // CLOSE
        supplyservo_deg = supplyservo.read(); //現在の角度を取得
    }

    if (supply_deg == 90)
    {
        for (main_deg = 0; main_deg <= 90; main_deg++)
        {
            mainservo.write(main_deg);   // OPEN
            mainservo_deg = mainservo.read(); //現在の角度を取得
        }
    }
    Serial.println("-- Complete Lunch Position --");
    Serial.println();
}

/*
void WAITING_Position()
{
    unsigned long curr_SERVO;

    for (deg = 90; deg >= 0; deg--)
    {
        supplyservo.write(deg);               // OPEN
        supplyservo_deg = supplyservo.read(); //現在の角度を取得
    }

    unsigned long curr_SERVO = millis();
    if ((curr_SERVO - prev_SERVO) >= interval_SERVO)
    {
        for (deg = 90; deg >= 0; deg--)
        {
            mainservo.write(deg);             // CLOSE
            mainservo_deg = mainservo.read(); //現在の角度を取得
        }
        //digitalWrite(3, LOW);
        Serial.println("== Complete WAITING Position ==");
        Serial.println();

        prev_SERVO = curr_SERVO;
    }
}
*/

void WAITING_Position()
{

    for (supply_deg = 90; supply_deg >= 0; supply_deg--)
    {
        supplyservo.write(supply_deg);   // OPEN
        supplyservo_deg = supplyservo.read(); //現在の角度を取得
    }

    if (supply_deg == 0)
    {
        for (main_deg = 90; main_deg >= 0; main_deg--)
        {
            mainservo.write(main_deg);             // CLOSE
            mainservo_deg = mainservo.read(); //現在の角度を取得
        }
    }
    Serial.println("== Complete WAITING Position ==");
    Serial.println();
}

void downlinkFlightData_tlm()
{
    if (!LoRa.begin(923E6))
        return;

    unsigned long curr_SEND = millis();
    if ((curr_SEND - prev_SEND) >= interval_SEND)
    {
        downPacket_tlm.clear();
        downPacket_tlm["sensor"] = "gps";
        downPacket_tlm["lat"] = String(latitude, 8);
        downPacket_tlm["lon"] = String(longitude, 8);
        downPacket_tlm["satellites"] = String(satellites, 1);
        downPacket_tlm["presentTime"] = String(Time, 10);
        downPacket_tlm["mainservoDeg"] = String(mainservo_deg, 1);
        downPacket_tlm["supplyservoDeg"] = String(supplyservo_deg, 1);

        LoRa.beginPacket();
        serializeJson(downPacket_tlm, LoRa);
        LoRa.endPacket();

        Serial.println();
        serializeJson(downPacket_tlm, Serial);
        prev_SEND = curr_SEND;
    }
}

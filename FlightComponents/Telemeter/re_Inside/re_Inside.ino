#include <LoRa.h>
#include <SPI.h>
#include <Arduino_MKRGPS.h>
#include <Servo.h>
#include <ArduinoJson.h>

//サーボの設定
Servo mainservo;
Servo supplyservo;
volatile int main_min_deg = 0;
volatile int supply_min_deg = 0;
volatile int main_max_deg = 90;
volatile int supply_max_deg = 90;
volatile int t = 1;
float supplyservo_deg;
float mainservo_deg;

int pos = 0;
//  int min_deg = 0;
//  int max_deg = 90;

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
// unsigned long interval_SERVO, prev_SERVO;

void setup()
{

    //スイッチピンの設定
    pinMode(6, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(6), L_Position, RISING);

    pinMode(8, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(8), W_Position, RISING);

    // pinMode(6, INPUT_PULLUP);
    // attachInterrupt(digitalPinToInterrupt(6), LUNCH_Status, RISING);

    // pinMode(8, INPUT_PULLUP);
    // attachInterrupt(digitalPinToInterrupt(8), W_Status, RISING);

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

    switch (t)
    {
    case 90:
        detachInterrupt(6);

        delayMicroseconds(1000000);
        for (pos = 0; pos <= main_max_deg; pos += 1)
        {
            mainservo.write(pos);
            Serial.println(pos);
            delayMicroseconds(17000);
        }

        t = 1;

        Serial.println("Comp LUNCH Position");
        Serial.println();
        break;

    case 0:
        detachInterrupt(8);

        delayMicroseconds(1000000);
        for (pos = main_max_deg; pos >= main_min_deg; pos -= 1)
        {
            mainservo.write(pos);
            Serial.println(pos);
            delayMicroseconds(17000);
        }
        
        t = 1;
        
        Serial.println("Comp WAITING Position");
        Serial.println();

    default:
        break;
    }
}

void L_Position()
{
    supplyservo.write(supply_max_deg);
    t = supply_max_deg;
    supplyservo_deg = supplyservo.read();
    Serial.print("Supply_Degree");
    Serial.println(supplyservo_deg);
}

void W_Position()
{
    supplyservo.write(supply_min_deg);
    t = supply_min_deg;
    supplyservo_deg = supplyservo.read();
    Serial.print("Supply_Degree");
    Serial.println(supplyservo_deg);
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

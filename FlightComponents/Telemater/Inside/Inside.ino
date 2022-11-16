/*
  機体からGPS情報を地上に送信するArduinoです。
*/

#include <LoRa.h>
#include <SPI.h>
#include <Arduino_MKRGPS.h>
#include <Servo.h>

//サーボの設定
Servo mainservo;
Servo supplyservo;
volatile int deg = 0;
// int min_deg = 0;
// int max_deg = 90;

// GPSの設定
float latitude;
float longitude;
float altitude;
float speed;
float satellites;

byte localAddress = 0xBB; // address of this device
byte destination = 0xFF;  // where we are sending data to

//電圧検知の設定
// const int VoltageDetectionPin = A1; // A1を電圧検知入力ピンに設定
// int value;
// float volt;

//スイッチの設定
// const int SwitchPin = 6;
// int value;
unsigned long prev_SEND, prev_PRINT, interval_SEND, interval_PRINT;

void setup()
{

    // initialize serial communications and wait for port to open:
    Serial.begin(9600);
    // while (!Serial)

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
    interval_SEND = 3000;

    prev_PRINT = 0;
    interval_PRINT = 5000;


    //スイッチピンの設定
    pinMode(6, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(6), LUNCH_Position, RISING);

    pinMode(7, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(7), WAITING_Position, RISING);
}

void loop()
{
    // check if there is new GPS data available
    if (GPS.available()) //&& mainservo.read() == 90
    {
        // read GPS values
        latitude = GPS.latitude();
        longitude = GPS.longitude();
        altitude = GPS.altitude();
        speed = GPS.speed();
        satellites = GPS.satellites();

        // print GPS values
        printValues();

        // Create and send LoRa packet
        LoRa_send();
    }
    // receiverOpen();

    // Serial.println(mainservo.read());
}

// function to send information over LoRa network
void LoRa_send()
{
    /*
    LoRa.beginPacket();      // creates a LoRa packet
    LoRa.write(destination); // destination address
    LoRa.print("Location ");
    // LoRa.print("LAT: ");
    LoRa.print(latitude, 8); //国土地理院の地理院地図を参考に決定
    // LoRa.print(" LONG: ");
    LoRa.print(",");
    LoRa.println(longitude, 8); // 5桁目以降の表示は非推奨
    LoRa.print(speed);
    LoRa.println(" km/h");
    LoRa.print(altitude);
    LoRa.println("m");
    LoRa.print("Numder of satellites: ");
    LoRa.println(satellites);
    LoRa.println();
    LoRa.endPacket(); // sends the LoRa packet
    */
    //delay(10000);     // a 10 second delay to limit the amount of packets sent
    unsigned long curr_SEND = millis();
    if ((curr_SEND - prev_SEND) >= interval_SEND)
    {
        LoRa.beginPacket();      // creates a LoRa packet
        LoRa.write(destination); // destination address
        LoRa.print("Location ");
        // LoRa.print("LAT: ");
        LoRa.print(latitude, 8); //国土地理院の地理院地図を参考に決定
        // LoRa.print(" LONG: ");
        LoRa.print(",");
        LoRa.println(longitude, 8); // 5桁目以降の表示は非推奨
        LoRa.print(speed);
        LoRa.println(" km/h");
        LoRa.print(altitude);
        LoRa.println("m");
        LoRa.print("Numder of satellites: ");
        LoRa.println(satellites);
        LoRa.println();
        LoRa.endPacket(); // sends the LoRa packet
        prev_SEND = curr_SEND;
    }
}

// function that prints all readings in the Serial Monitor

void printValues()
{
    unsigned long curr_PRINT = millis();
    if ((curr_PRINT - prev_PRINT) >= interval_PRINT)
    {
    Serial.print("Location: ");
    Serial.print(latitude, 8);
    Serial.print(", ");
    Serial.println(longitude, 8);
    Serial.print("Altitude: ");
    Serial.print(altitude);
    Serial.println("m");
    Serial.print("Ground speed: ");
    Serial.print(speed);
    Serial.println(" km/h");
    Serial.print("Number of satellites: ");
    Serial.println(satellites);
    Serial.println();

    prev_PRINT = curr_PRINT;
    }
}

// Create Reciver code.
/*
void receiverOpen()
{
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        byte Opencommand = LoRa.read();

        switch (Opencommand)
        {
        case 0x00:

            Serial.println("CLOSE");
            for (int deg = max_deg; deg >= min_deg; deg -= 1)
            {
                supplyservo.write(deg);
            }

            for (int deg = max_deg; deg >= min_deg; deg -= 1)
            {
                mainservo.write(deg);
            }

            break;

        case 0x01:

            Serial.println("OPEN");
            for (int deg = min_deg; deg <= max_deg; deg += 1)
            {
                supplyservo.write(deg);
            }

            for (int deg = min_deg; deg <= max_deg; deg += 1)
            {
                mainservo.write(deg);
            }

            break;

        default:
            mainservo.write(min_deg);
            supplyservo.write(min_deg);
            break;
        }
    }
}
*/

// Wired connection function
/*
void Switch()
{
    value = digitalRead(SwitchPin);

    if (value == HIGH) // LUNCH Position
    {
        for (int deg = 0; deg <= 90; deg += 1)
        {
            supplyservo.write(deg); // CLOSE
        }
        // Serial.println("== Supply CLOSE ==");

        for (int deg = 0; deg <= 90; deg += 1)
        {
            mainservo.write(deg); // OPEN
        }
        // Serial.println("== Main OPEN ==");
        // Serial.println("-- Complete Lunch Position --");
    }
    else // Waiting Position
    {
        for (int deg = 90; deg <= 0; deg -= 1)
        {
            supplyservo.write(deg); // OPEN
        }
        // Serial.println("== Supply OPEN ==");

        for (int deg = 90; deg <= 0; deg -= 1)
        {
            mainservo.write(deg); // CLOSE
        }
        // Serial.println("== Main CLOSE ==");
        // Serial.println("-- Complete Waiting Position --");
    }
    delay(100);
}
*/
void LUNCH_Position()
{
    for (deg = 0; deg <= 90; deg++)
    {
        supplyservo.write(deg); // CLOSE
    }
    // Serial.println("== Supply CLOSE ==");

    for (deg = 0; deg <= 90; deg++)
    {
        mainservo.write(deg); // OPEN
    }
    // Serial.println("== Main OPEN ==");
    Serial.println("-- Complete Lunch Position --");
}

void WAITING_Position()
{
    for (deg = 90; deg >= 0; deg--)
    {
        supplyservo.write(deg); // OPEN
    }

    for (deg = 90; deg >= 0; deg--)
    {
        mainservo.write(deg); // CLOSE
    }
    Serial.println("== Complete WAITING Position ==");
}
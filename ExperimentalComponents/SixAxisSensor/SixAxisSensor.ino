#include <Wire.h>
#include <MultiUART.h>
#include <Arduino_JSON.h>
#include <MPU6050_6Axis_MotionApps20.h>

MPU6050 SixAxisSensor;

MultiUART PC(Serial);
MultiUART SD(2, 3);

int16_t Ax, Ay, Az;
int16_t Gx, Gy, Gz;

float AccelScaleFactor = 2048.0;
float GyroScaleFactor = 16.4;

void setup()
{
  Wire.begin();
  PC.begin(9600);
  SD.begin(9600);

  while (!SD)
    ;

  SixAxisSensor.initialize();

  SixAxisSensor.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
  SixAxisSensor.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
}

void loop()
{
  SixAxisSensor.getMotion6(&Ax, &Ay, &Az, &Gx, &Gy, &Gz);

  SD.print(millis());
  SD.print("\t");
  SD.print(Ax / AccelScaleFactor);
  SD.print("\t");
  SD.print(Ay / AccelScaleFactor);
  SD.print("\t");
  SD.print(Az / AccelScaleFactor);
  SD.print("\t");
  SD.print(Gx / GyroScaleFactor);
  SD.print("\t");
  SD.print(Gy / GyroScaleFactor);
  SD.print("\t");
  SD.println(Gz / GyroScaleFactor);

  delay(10);
}

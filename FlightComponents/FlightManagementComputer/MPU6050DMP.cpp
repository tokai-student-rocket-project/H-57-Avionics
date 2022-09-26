#include <Arduino.h>
#include "MPU6050DMP.h"

MPU6050DMP::MPU6050DMP() {
}

void MPU6050DMP::initialize() {
  _mpu6050.initialize();
  _mpu6050.dmpInitialize();
  _mpu6050.setDMPEnabled(true);
}

void MPU6050DMP::getYpr(double *yaw, double *pitch, double *roll) {
      float ypr[3];

      _mpu6050.dmpGetQuaternion(&_quaternion, _fifoBuffer);
      _mpu6050.dmpGetGravity(&_gravity, &_quaternion);
      _mpu6050.dmpGetYawPitchRoll(ypr, &_quaternion, &_gravity);

      *yaw = (double)ypr[0];
      *pitch = (double)ypr[1];
      *roll = (double)ypr[2];
    }
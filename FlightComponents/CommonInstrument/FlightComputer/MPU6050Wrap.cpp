#include <Arduino.h>
#include "MPU6050Wrap.h"


void MPU6050Wrap::initialize() {
  _mpu6050.initialize();
  _mpu6050.dmpInitialize();

  _mpu6050.CalibrateAccel(6);
  _mpu6050.CalibrateGyro(6);
  _mpu6050.setDMPEnabled(true);
  _packetSize = _mpu6050.dmpGetFIFOPacketSize();
}


void MPU6050Wrap::update() {
  if (_mpu6050.dmpGetCurrentFIFOPacket(_fifoBuffer)) {
    _mpu6050.dmpGetQuaternion(&_q, _fifoBuffer);
    _mpu6050.dmpGetGravity(&_gravity, &_q);

    _mpu6050.dmpGetAccel(&_aa, _fifoBuffer);
    _mpu6050.dmpGetLinearAccel(&_aaReal, &_aa, &_gravity);
    _accelerationX = _aaReal.x;
    _accelerationY = _aaReal.y;
    _accelerationZ = _aaReal.z;

    float ypr[3];
    _mpu6050.dmpGetYawPitchRoll(ypr, &_q, &_gravity);
    _yaw = ypr[0] * 180 / M_PI;
    _pitch = ypr[1] * 180 / M_PI;
    _roll = ypr[2] * 180 / M_PI;
  }
}


float MPU6050Wrap::getAccelerationX() {
  return _accelerationX;
}


float MPU6050Wrap::getAccelerationY() {
  return _accelerationY;
}


float MPU6050Wrap::getAccelerationZ() {
  return _accelerationZ;
}


float MPU6050Wrap::getYaw() {
  return _yaw;
}


float MPU6050Wrap::getPitch() {
  return _pitch;
}


float MPU6050Wrap::getRoll() {
  return _roll;
}
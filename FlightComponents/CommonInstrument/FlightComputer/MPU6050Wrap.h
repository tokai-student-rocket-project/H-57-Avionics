#ifndef MPU6050_Wrap_H_
#define MPU6050_Wrap_H_


#include <MPU6050_6Axis_MotionApps_V6_12.h>


class MPU6050Wrap {
private:
  MPU6050 _mpu6050;

  uint16_t _packetSize;
  uint8_t _fifoBuffer[64];

  Quaternion _q;
  VectorInt16 _aa;
  VectorInt16 _aaReal;
  VectorFloat _gravity;

  float _accelerationX;
  float _accelerationY;
  float _accelerationZ;

  float _yaw;
  float _pitch;
  float _roll;

public:
  void initialize();

  void update();

  float getAccelerationX();
  float getAccelerationY();
  float getAccelerationZ();

  float getYaw();
  float getPitch();
  float getRoll();
};


#endif
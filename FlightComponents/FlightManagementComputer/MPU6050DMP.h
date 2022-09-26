#ifndef MPU6050_DMP_H_
#define MPU6050_DMP_H_

#include <MPU6050_6Axis_MotionApps20.h>

/// @brief LEDの抽象化クラス
class MPU6050DMP
{
public:
  MPU6050DMP();

  void initialize();

  void getYpr(double *yaw, double *pitch, double *roll);
  
private:
  const MPU6050 _mpu6050;
  Quaternion _quaternion;
  uint8_t _fifoBuffer[64];
  VectorFloat _gravity;
};

#endif
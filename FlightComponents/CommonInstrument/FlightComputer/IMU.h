#ifndef IMU_H_
#define IMU_H_


#include <MPU6050.h>
#include <MadgwickAHRS.h>

class IMU {
private:
  MPU6050 _mpu6050;

  // 姿勢角算出用のフィルタ
  Madgwick _madgwickFilter;

public:
  void initialize();

  void getData(
    float* accelerationX, float* accelerationY, float* accelerationZ,
    float* gyroX, float* gyroY, float* gyroZ,
    float* yaw, float* pitch, float* roll
  );
};


#endif
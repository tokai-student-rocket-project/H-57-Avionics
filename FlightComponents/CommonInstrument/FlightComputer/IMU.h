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
  void initialize(
    int16_t offsetAccelerationX, int16_t offsetAccelerationY, int16_t offsetAccelerationZ,
    int16_t offsetGyroX, int16_t offsetGyroY, int16_t offsetGyroZ
  );

  void getData(
    float* accelerationX, float* accelerationY, float* accelerationZ,
    float* gyroX, float* gyroY, float* gyroZ,
    float* yaw, float* pitch, float* roll
  );
};


#endif
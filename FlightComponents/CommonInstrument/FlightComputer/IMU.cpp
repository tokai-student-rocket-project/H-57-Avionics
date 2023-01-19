#include <Arduino.h>
#include "IMU.h"


void IMU::initialize(
  int16_t offsetAccelerationX, int16_t offsetAccelerationY, int16_t offsetAccelerationZ,
  int16_t offsetGyroX, int16_t offsetGyroY, int16_t offsetGyroZ
) {
  _mpu6050.initialize();

  // +-16[G]。2048[LBS/G]
  _mpu6050.setFullScaleAccelRange(MPU6050_IMU::ACCEL_FS::MPU6050_ACCEL_FS_16);
  // +-2000[deg/s]。16.4[LBS/(deg/s)]
  _mpu6050.setFullScaleGyroRange(MPU6050_IMU::GYRO_FS::MPU6050_GYRO_FS_2000);

  // _mpu6050.CalibrateAccel();
  // _mpu6050.CalibrateGyro();

  // センサ固有のオフセット。mpu6050ライブラリのIMU_ZEROから求める
  _mpu6050.setXAccelOffset(offsetAccelerationX);
  _mpu6050.setYAccelOffset(offsetAccelerationY);
  _mpu6050.setZAccelOffset(offsetAccelerationZ);
  _mpu6050.setXGyroOffset(offsetGyroX);
  _mpu6050.setYGyroOffset(offsetGyroY);
  _mpu6050.setYGyroOffset(offsetGyroZ);

  _madgwickFilter.begin(100);
}


void IMU::getData(
  float* accelerationX, float* accelerationY, float* accelerationZ,
  float* gyroX, float* gyroY, float* gyroZ,
  float* yaw, float* pitch, float* roll
) {
  int16_t lbsAccelerationX, lbsAccelerationY, lbsAccelerationZ;
  int16_t lbsGyroX, lbsGyroY, lbsGyroZ;

  _mpu6050.getMotion6(
    &lbsAccelerationX, &lbsAccelerationY, &lbsAccelerationZ,
    &lbsGyroX, &lbsGyroY, &lbsGyroZ);

  *accelerationX = lbsAccelerationX / 2048.0;
  *accelerationY = lbsAccelerationY / 2048.0;
  *accelerationZ = lbsAccelerationZ / 2048.0;

  *gyroX = lbsGyroX / 16.4;
  *gyroY = lbsGyroY / 16.4;
  *gyroZ = lbsGyroZ / 16.4;

  _madgwickFilter.updateIMU(
    *gyroX, *gyroY, *gyroZ,
    *accelerationX, *accelerationY, *accelerationZ
  );

  *yaw = _madgwickFilter.getYaw();
  *pitch = _madgwickFilter.getPitch();
  *roll = _madgwickFilter.getRoll();
}
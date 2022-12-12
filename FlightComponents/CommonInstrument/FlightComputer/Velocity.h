#ifndef VELOCITY_H_
#define VELOCITY_H_


class Velocity {
private:
  const float G_mps2 = 9.80665;

  uint32_t _t_ref_ms = 0;
  float _vx_ref_mps = 0;
  float _vy_ref_mps = 0;
  float _vz_ref_mps = 0;

public:
  void initialize();
  float getSpeed(
    float acceleration_x_g,
    float acceleration_y_g,
    float acceleration_z_g,
    float acceleration_x_average_g,
    float acceleration_y_average_g,
    float acceleration_z_average_g);
};


#endif
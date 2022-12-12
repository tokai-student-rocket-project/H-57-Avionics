#include <Arduino.h>
#include "Velocity.h"


void Velocity::initialize() {
  _t_ref_ms = 0;
  _vx_ref_mps = 0;
  _vy_ref_mps = 0;
  _vz_ref_mps = 0;
}


float Velocity::getSpeed(
  float acceleration_x_g,
  float acceleration_y_g,
  float acceleration_z_g,
  float acceleration_x_average_g,
  float acceleration_y_average_g,
  float acceleration_z_average_g)
{
  uint32_t t_ms = millis();
  uint32_t dt_ms = t_ms - _t_ref_ms;
  _t_ref_ms = t_ms;

  if (dt_ms >= 10) {
    float acceleration_x_world_g = acceleration_x_g - acceleration_x_average_g;
    float acceleration_y_world_g = acceleration_y_g - acceleration_y_average_g;
    float acceleration_z_world_g = acceleration_z_g - acceleration_z_average_g;

    float dt_s = dt_ms / 1000.0;
    float dvx_mps = acceleration_x_world_g * G_mps2 * dt_s;
    float dvy_mps = acceleration_y_world_g * G_mps2 * dt_s;
    float dvz_mps = acceleration_z_world_g * G_mps2 * dt_s;

    _vx_ref_mps += dvx_mps;
    _vy_ref_mps += dvy_mps;
    _vz_ref_mps += dvz_mps;
  }

  return sqrt(pow(_vx_ref_mps, 2) + pow(_vy_ref_mps, 2) + pow(_vz_ref_mps, 2));
}
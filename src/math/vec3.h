#pragma once

#include <cmath>

struct Vec3 {
  float x, y, z;
};

namespace vec3 {

inline Vec3 rotate_x(const Vec3& v, const float angle) {
  return {
      v.x,
      v.y * cos(angle) - v.z * sin(angle),
      v.y * sin(angle) + v.z * cos(angle)
  };
}

inline Vec3 rotate_y(const Vec3& v, const float angle) {
  return {
      v.x * cos(angle) - v.z * sin(angle),
      v.y,
      v.x * sin(angle) + v.z * cos(angle)
  };
}

inline Vec3 rotate_z(const Vec3& v, const float angle) {
  return {
      v.x * cos(angle) - v.y * sin(angle),
      v.x * sin(angle) + v.y * cos(angle),
      v.z
  };
}

} // namespace vec3


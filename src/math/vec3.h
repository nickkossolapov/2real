#pragma once

#include <cmath>

namespace math {

struct Vec3 {
  float x = 0.0f, y = 0.0f, z = 0.0f;

  float length() const {
    return std::sqrt(x * x + y * y + z * z);
  }

  Vec3 operator+(const Vec3& v) const {
    return {x + v.x, y + v.y, z + v.z};
  }

  Vec3& operator+=(const Vec3& v) {
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
  }

  Vec3& operator+=(const float f) {
    x += f;
    y += f;
    z += f;

    return *this;
  }

  Vec3 operator-(const Vec3& v) const {
    return {x - v.x, y - v.y, z - v.z};
  }

  Vec3& operator-=(const Vec3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;

    return *this;
  }

  Vec3& operator-=(const float f) {
    x -= f;
    y -= f;
    z -= f;

    return *this;
  }

  Vec3 operator*(const float s) const {
    return {x * s, y * s, z * s};
  }

  Vec3 operator*=(const float s) {
    x *= s;
    y *= s;
    z *= s;

    return *this;
  }

  Vec3 operator/(const float s) const {
    return {x / s, y / s, z / s};
  }

  Vec3 operator/=(const float s) {
    x /= s;
    y /= s;
    z /= s;

    return *this;
  }

  Vec3 operator-() const {
    return {-x, -y, -z};
  }
};

inline float dot(const Vec3& v1, const Vec3& v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vec3 cross(const Vec3& v1, const Vec3& v2) {
  return {
      v1.y * v2.z - v1.z * v2.y,
      v1.z * v2.x - v1.x * v2.z,
      v1.x * v2.y - v1.y * v2.x
  };
}

} // namespace math

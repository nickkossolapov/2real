#pragma once

#include <cmath>

namespace math {

struct Vec3 {
  float x = 0.0f, y = 0.0f, z = 0.0f;

  float length() const {
    return std::sqrt(x * x + y * y + z * z);
  }

  void normalize() {
    constexpr static float tol = 1e-6f;

    if (const float m = length(); m > tol) {
      x /= m;
      y /= m;
      z /= m;
    } else {
      x = 0.0f;
      y = 0.0f;
      z = 0.0f;
    }

    if (std::fabs(x) < tol) { x = 0.0f; }
    if (std::fabs(y) < tol) { y = 0.0f; }
    if (std::fabs(z) < tol) { z = 0.0f; }
  }

  Vec3 normalized() const {
    Vec3 v = {x, y, z};
    v.normalize();

    return v;
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

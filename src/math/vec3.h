#pragma once

#include <cmath>

struct Vec3 {
  float x = 0, y = 0, z = 0;

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

  Vec3 operator-(const Vec3& v) const {
    return {x - v.x, y - v.y, z - v.z};
  }

  Vec3& operator-=(const Vec3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;

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

namespace vec3 {

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

inline Vec3 rotate(const Vec3& v, const Vec3& rot) {
  return rotate_x(rotate_y(rotate_z(v, rot.z), rot.y), rot.x);
}

} // namespace vec3

#pragma once
#include "common.h"

#include <cassert>
#include <cmath>

namespace math {

struct Vec2 {
  float x = 0.0f, y = 0.0f;

  float length() const { return std::sqrt(x * x + y * y); }

  float length_squared() const { return x * x + y * y; }

  Vec2 operator+(const Vec2& v) const { return {x + v.x, y + v.y}; }

  Vec2& operator+=(const Vec2& v) {
    x += v.x;
    y += v.y;

    return *this;
  }

  Vec2 operator-(const Vec2& v) const { return {x - v.x, y - v.y}; }

  Vec2& operator-=(const Vec2& v) {
    x -= v.x;
    y -= v.y;

    return *this;
  }

  Vec2 operator*(const float s) const { return {x * s, y * s}; }

  Vec2& operator*=(const float s) {
    x *= s;
    y *= s;

    return *this;
  }

  Vec2 operator/(const float s) const { return {x / s, y / s}; }

  Vec2& operator/=(const float s) {
    x /= s;
    y /= s;

    return *this;
  }

  Vec2 operator-() const { return {-x, -y}; }

  Vec2 normalized() const {
    const float m = length();

    assert(m >= epsilon);

    if (m < epsilon) {
      return {0, 0};
    }

    return {x / m, y / m};
  }
};

inline float dot(const Vec2& v1, const Vec2& v2) {
  return v1.x * v2.x + v1.y * v2.y;
}

} // namespace math

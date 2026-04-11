#pragma once
#include <cmath>

struct Vec2 {
  float x, y;

  float length() const {
    return std::sqrt(x * x + y * y);
  }

  Vec2 operator+(const Vec2& v) const {
    return {x + v.x, y + v.y};
  }

  Vec2& operator+=(const Vec2& v) {
    x += v.x;
    y += v.y;

    return *this;
  }

  Vec2 operator-(const Vec2& v) const {
    return {x - v.x, y - v.y};
  }

  Vec2& operator-=(const Vec2& v) {
    x -= v.x;
    y -= v.y;

    return *this;
  }

  Vec2 operator*(const float s) const {
    return {x * s, y * s};
  }

  Vec2 operator*=(const float s) {
    x *= s;
    y *= s;

    return *this;
  }

  Vec2 operator/(const float s) const {
    return {x / s, y / s};
  }

  Vec2 operator/=(const float s) {
    x /= s;
    y /= s;

    return *this;
  }

  Vec2 operator-() const {
    return {-x, -y};
  }
};

namespace vec2 {

inline float dot(const Vec2& v1, const Vec2& v2) {
  return v1.x * v2.x + v1.y * v2.y;
}

inline float cross(const Vec2& v1, const Vec2& v2) {
  return v1.x * v2.y - v1.y * v2.x; // returns the z component of the cross product
}

} // namespace vec2

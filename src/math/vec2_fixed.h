#pragma once
#include "fixed.h"
#include "vec2.h"

namespace math {

struct Vec2Fixed {
  Fixed x{0}, y{0};

  explicit Vec2Fixed(const Vec2& v)
      : x(Fixed{v.x}),
        y(Fixed{v.y}) {}

  explicit Vec2Fixed(const Fixed& x, const Fixed& y)
      : x(x),
        y(y) {}

  explicit Vec2Fixed(const int x, const int y)
      : x(x),
        y(y) {}

  Vec2Fixed operator+(const Vec2Fixed& v) const { return Vec2Fixed{x + v.x, y + v.y}; }

  Vec2Fixed& operator+=(const Vec2Fixed& v) {
    x += v.x;
    y += v.y;

    return *this;
  }

  Vec2Fixed operator-(const Vec2Fixed& v) const { return Vec2Fixed{x - v.x, y - v.y}; }

  Vec2Fixed& operator-=(const Vec2Fixed& v) {
    x -= v.x;
    y -= v.y;

    return *this;
  }
};

inline Fixed dot(const Vec2Fixed& v1, const Vec2Fixed& v2) {
  return v1.x * v2.x + v1.y * v2.y;
}

inline Fixed cross(const Vec2Fixed& v1, const Vec2Fixed& v2) {
  return v1.x * v2.y - v1.y * v2.x; // returns the z component of the 3D cross product
}

} // namespace math

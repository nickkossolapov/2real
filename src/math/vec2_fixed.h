#pragma once

namespace math {

// TODO move to actual fixed point arithmatic
struct Vec2Fixed {
  int x = 0, y = 0;

  Vec2Fixed operator+(const Vec2Fixed& v) const { return {x + v.x, y + v.y}; }

  Vec2Fixed& operator+=(const Vec2Fixed& v) {
    x += v.x;
    y += v.y;

    return *this;
  }

  Vec2Fixed operator-(const Vec2Fixed v) const { return {x - v.x, y - v.y}; }

  Vec2Fixed& operator-=(const Vec2Fixed& v) {
    x -= v.x;
    y -= v.y;

    return *this;
  }

  Vec2Fixed operator-() const { return {-x, -y}; }
};

inline float dot(const Vec2Fixed& v1, const Vec2Fixed& v2) {
  return v1.x * v2.x + v1.y * v2.y;
}

inline float cross(const Vec2Fixed& v1, const Vec2Fixed& v2) {
  return v1.x * v2.y - v1.y * v2.x; // returns the z component of the 3D cross product
}

} // namespace math

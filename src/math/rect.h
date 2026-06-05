#pragma once

#include "vec2_fixed.h"

#include <algorithm>

namespace math {

struct Rect {
  int x_min;
  int x_max;
  int y_min;
  int y_max;
};

inline Rect bounding_box(const Vec2Fixed& a, const Vec2Fixed& b, const Vec2Fixed& c) {
  return {
      .x_min = std::min({a.x, b.x, c.x}),
      .x_max = std::max({a.x, b.x, c.x}),
      .y_min = std::min({a.y, b.y, c.y}),
      .y_max = std::max({a.y, b.y, c.y}),
  };
}

} // namespace math
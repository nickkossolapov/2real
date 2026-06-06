#pragma once

#include "vec2_fixed.h"

#include <algorithm>
#include <vector>

namespace math {

struct Rect {
  int x_min;
  int x_max;
  int y_min;
  int y_max;
};

inline Rect bounding_box(const Vec2& a, const Vec2& b, const Vec2& c) {
  return {
      .x_min = floor_to_int(std::min({a.x, b.x, c.x})),
      .x_max = ceil_to_int(std::max({a.x, b.x, c.x})),
      .y_min = floor_to_int(std::min({a.y, b.y, c.y})),
      .y_max = ceil_to_int(std::max({a.y, b.y, c.y})),
  };
}

} // namespace math
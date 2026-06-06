#pragma once

#include "vec2_fixed.h"

#include <algorithm>
#include <vector>

namespace math {

template <typename T> struct Rect {
  T x_min;
  T x_max;
  T y_min;
  T y_max;
};

inline Rect<Fixed> bounding_box(const std::array<Vec2Fixed, 3>& v) {
  return {
      .x_min = min(min(v[0].x, v[1].x), v[2].x),
      .x_max = max(max(v[0].x, v[1].x), v[2].x),
      .y_min = min(min(v[0].y, v[1].y), v[2].y),
      .y_max = max(max(v[0].y, v[1].y), v[2].y),
  };
}

} // namespace math
#pragma once

#include "vec2.h"

namespace math {

struct Rect {
  float x_min;
  float x_max;
  float y_min;
  float y_max;

  bool contains(const Vec2 point) const {
    return point.x >= x_min && point.x <= x_max && point.y >= y_min && point.y <= y_max;
  }
};

} // namespace math

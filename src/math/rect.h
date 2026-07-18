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

  bool intersects(const Rect b) const {
    if (x_max < b.x_min || x_min > b.x_max || y_max < b.y_min || y_min > b.y_max) {
      return false;
    }

    return true;
  }
};

} // namespace math

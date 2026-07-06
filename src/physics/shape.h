#pragma once

#include "util/overload.h"

#include <variant>
#include <vector>

namespace physics {

namespace shape {

struct Circle {
  float radius;
};

struct Polygon {
  const std::vector<math::Vec2> points;
};

static Polygon box(const float width, const float height) {
  return {{
      {-width / 2.0f, height / 2.0f},
      {-width / 2.0f, -height / 2.0f},
      {width / 2.0f, -height / 2.0f},
      {width / 2.0f, height / 2.0f},
  }};
}

} // namespace shape

using Shape = std::variant<shape::Circle, shape::Polygon>;

inline float compute_moment_of_inertia(const Shape& shape, const float mass) {
  auto visitor = Overloaded{
      [&](const shape::Circle& c) { return 0.5f * mass * c.radius * c.radius; },
      [&](const shape::Polygon& p) { return 0.0f; },
  };

  return std::visit(visitor, shape);
}

} // namespace physics
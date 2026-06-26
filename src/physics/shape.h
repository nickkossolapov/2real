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
  std::vector<math::Vec2> points;
};

} // namespace shape

using Shape = std::variant<shape::Circle, shape::Polygon>;

inline float compute_moment_of_inertia(const Shape& shape, const float mass) {
  auto visitor = Overloaded{
      [&](const shape::Circle& c) { return 0.5f * mass * c.radius * c.radius; },
      [&](const shape::Polygon& p) { return 1.0f; }, // TODO actually calculate
  };

  return std::visit(visitor, shape);
}

} // namespace physics
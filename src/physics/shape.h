#pragma once

#include "util/overload.h"

#include <variant>
#include <vector>

namespace physics {

namespace shape {

struct Circle {
  float radius;
};

struct Box {
  float width;
  float height;

  const std::vector<math::Vec2> points;

  explicit Box(const float width, const float height)
      : width(width),
        height(height),
        points({
            {-width / 2.0f, height / 2},
            {width / 2.0f, height / 2},
            {width / 2.0f, -height / 2},
            {-width / 2.0f, -height / 2},
        }) {}
};

} // namespace shape

using Shape = std::variant<shape::Circle, shape::Box>;

inline float compute_moment_of_inertia(const Shape& shape, const float mass) {
  auto visitor = Overloaded{
      [&](const shape::Circle& c) { return 0.5f * mass * c.radius * c.radius; },
      [&](const shape::Box& b) { return 1.0f / 12.0f * mass * (b.width * b.width + b.height * b.height); },
  };

  return std::visit(visitor, shape);
}

} // namespace physics
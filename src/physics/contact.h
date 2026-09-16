#pragma once

namespace physics {

struct Contact {
  math::Vec2 point_a;
  math::Vec2 point_b;

  math::Vec2 normal;
  float depth;

  Contact flipped() const {
    return {
        .point_a = point_b,
        .point_b = point_a,
        .normal = -normal,
        .depth = depth,
    };
  }
};

} // namespace physics

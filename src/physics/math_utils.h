#pragma once
#include "math/vec2.h"

// 2D vector functions that probably won't have much applicability to 3D physics,
// so I'm putting them here instead of in Vec2.h to avoid crowding it with unused functions

namespace physics::math_utils {

inline float cross(const math::Vec2& v1, const math::Vec2& v2) {
  return v1.x * v2.y - v1.y * v2.x; // returns the z component of the cross product
}

/// Right/clockwise perpendicular (y, -x)
inline math::Vec2 right_perpendicular(const math::Vec2& v) {
  return {v.y, -v.x};
}

/// Outward normal of a CW-wound polygon edge
inline math::Vec2 normal(const math::Vec2& v) {
  return math::Vec2(-v.y, v.x).normalized();
}

inline std::vector<math::Vec2> to_world(const math::Vec2 position,
                                        const std::vector<math::Vec2>& local,
                                        const float a) {
  std::vector<math::Vec2> world_points;

  for (int i = 0; i < local.size(); ++i) {
    const auto [x, y] = local[i];

    // Left-handed rotation matrix
    const float world_x = std::cos(a) * x + std::sin(a) * y + position.x;
    const float world_y = -std::sin(a) * x + std::cos(a) * y + position.y;

    world_points.emplace_back(world_x, world_y);
  }

  return world_points;
}

} // namespace physics::math_utils
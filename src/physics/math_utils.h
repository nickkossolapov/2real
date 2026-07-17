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

} // namespace physics::math_utils
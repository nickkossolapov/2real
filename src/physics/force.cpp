#include "force.h"

#include "math/common.h"

namespace physics::force {

math::Vec2 drag(const math::Vec2& velocity, const float k) {
  return -velocity * velocity.length() * k;
}

math::Vec2 friction(const math::Vec2& normal, const float k) {
  return -normal * k;
}

math::Vec2 spring(const math::Vec2& position, const math::Vec2& anchor, const float rest_length, const float k) {
  const math::Vec2 displacement = anchor - position;
  const float length = displacement.length();

  if (length <= math::epsilon) {
    return {0, 0};
  }

  return displacement * (k * (length - rest_length) / length);
}

math::Vec2 gravity(const float mass, float g) {
  return {.y = -mass * g};
}

} // namespace physics::force
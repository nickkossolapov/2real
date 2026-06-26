#include "force.h"

#include "body.h"
#include "math/common.h"

namespace physics::force {

math::Vec2 drag(const math::Vec2& velocity, const float k) {
  return -velocity * velocity.length() * k;
}

math::Vec2 friction(const math::Vec2& velocity, const float k) {
  return -velocity * k;
}

math::Vec2 spring(const Body& a, const Body& b, const SpringParams params) {
  const math::Vec2 displacement = b.position - a.position;
  const math::Vec2 relative_velocity = a.velocity - b.velocity;

  const float length = displacement.length();

  if (length <= math::epsilon) {
    return {0, 0};
  }

  const float spring_term = params.k * (length - params.rest_length);
  const float damping_term = params.c * math::dot(relative_velocity, displacement) / length;

  return displacement * ((spring_term - damping_term) / length);
}

math::Vec2 gravity(const float mass, float g) {
  return {.y = -mass * g};
}

} // namespace physics::force
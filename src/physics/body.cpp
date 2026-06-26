#include "body.h"

namespace physics {

Body::Body(const float m, const Shape& shape, const math::Vec2 pos, const float rot)
    : position(pos),
      rotation(rot),
      mass(m > 0.0f ? m : 0.0f),
      inv_mass(m > 0.0f ? 1.0f / m : 0.0f),
      shape(shape),
      inertia(compute_moment_of_inertia(shape, mass)),
      inv_inertia(inertia > 0.0f ? 1.0f / inertia : 1.0f) {}

void Body::integrate(const float dt) {
  const math::Vec2 acceleration = net_force_ * inv_mass;

  velocity += acceleration * dt;
  position += velocity * dt;

  const float angular_acceleration = net_torque_ * inv_inertia;

  angular_velocity += angular_acceleration * dt;
  rotation += angular_velocity;

  reset();
}

void Body::add_force(const math::Vec2 force) {
  net_force_ += force;
}

void Body::add_torque(const float torque) {
  net_torque_ += torque;
}

void Body::reset() {
  net_force_ = {};
  net_torque_ = 0;
}

} // namespace physics
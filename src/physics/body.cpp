#include "body.h"

#include "math_utils.h"

namespace physics {

Body::Body(const float m, const Shape& shape, const math::Vec2 pos, const float rot)
    : Body(m, 0.0f, shape, pos, rot) {}

Body::Body(const float m, const float restitution, const Shape& shape, const math::Vec2 pos, const float rot)
    : Body(m, restitution, 0.2f, shape, pos, rot) {}

Body::Body(const float m,
           const float restitution,
           const float friction,
           const Shape& shape,
           const math::Vec2 pos,
           const float rot)
    : position(pos),
      rotation(rot),
      mass_(m > 0.0f ? m : 0.0f),
      inv_mass_(m > 0.0f ? 1.0f / m : 0.0f),
      shape_(shape),
      inertia_(compute_moment_of_inertia(shape, mass_)),
      inv_inertia_(inertia_ > 0.0f ? 1.0f / inertia_ : 0.0f),
      restitution(restitution),
      friction(friction) {}

void Body::integrate(const float dt, const math::Vec2 gravity) {
  if (is_static()) {
    return;
  }

  const math::Vec2 acceleration = gravity * gravity_scale + net_force_ * inv_mass_;

  velocity += acceleration * dt;
  position += velocity * dt;

  const float angular_acceleration = net_torque_ * inv_inertia_;

  angular_velocity += angular_acceleration * dt;
  rotation += angular_velocity * dt;

  reset();
}

void Body::add_force(const math::Vec2 force) {
  net_force_ += force;
}

void Body::add_torque(const float torque) {
  net_torque_ += torque;
}
void Body::add_impulse(const math::Vec2 j) {
  if (is_static()) {
    return;
  }

  velocity += j * inv_mass_;
}

void Body::add_impulse(const math::Vec2 j, const math::Vec2 r) {
  if (is_static()) {
    return;
  }

  velocity += j * inv_mass_;
  angular_velocity += math_utils::cross(j, r) * inv_inertia_;
}

void Body::reset() {
  net_force_ = {};
  net_torque_ = 0;
}

} // namespace physics
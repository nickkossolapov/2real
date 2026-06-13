#include "particle.h"

namespace physics {

Particle::Particle(const float m, const float r, const math::Vec2 pos)
    : position(pos),
      radius(r),
      mass(m > 0.0f ? m : 0.0f),
      inv_mass(m > 0.0f ? 1.0f / m : 0.0f) {}

void Particle::integrate(const float dt) {
  const math::Vec2 acceleration = net_force_ * inv_mass;

  velocity += acceleration * dt;
  position += velocity * dt;

  reset_forces();
}
void Particle::add_force(const math::Vec2 force) {
  net_force_ += force;
}

void Particle::reset_forces() {
  net_force_ = {};
}

} // namespace physics
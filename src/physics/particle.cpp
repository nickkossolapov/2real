#include "particle.h"

namespace physics {

void Particle::integrate(float dt) {
  velocity += acceleration * dt;
  position += velocity * dt;
}

} // namespace physics
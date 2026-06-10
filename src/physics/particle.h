#pragma once

#include "math/vec2.h"

namespace physics {

struct Particle {
  math::Vec2 position;
  math::Vec2 velocity;
  math::Vec2 acceleration;
  float mass = 1.0f;
};

} // namespace physics

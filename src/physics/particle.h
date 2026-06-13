#pragma once

#include "math/vec2.h"

namespace physics {

struct Particle {
  math::Vec2 position;
  math::Vec2 velocity;
  const float radius;
  const float mass;
  const float inv_mass;

  explicit Particle(float m, float r, math::Vec2 pos = {});

  void integrate(float dt);
  void add_force(math::Vec2 force);

private:
  math::Vec2 net_force_;

  void reset_forces();
};

} // namespace physics

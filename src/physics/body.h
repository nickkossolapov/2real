#pragma once

#include "math/vec2.h"
#include "shape.h"

#include <variant>

namespace physics {

struct Body {
  math::Vec2 position;
  math::Vec2 velocity;

  float rotation = 0;
  float angular_velocity = 0;

  const float mass;
  const float inv_mass;

  const Shape shape;

  const float inertia;
  const float inv_inertia;

  explicit Body(float m, const Shape& shape, math::Vec2 pos = {}, float rot = 0);

  void integrate(float dt);
  void add_force(math::Vec2 force);
  void add_torque(float torque);

private:
  math::Vec2 net_force_;
  float net_torque_ = 0.0f;

  void reset_forces();
};

} // namespace physics

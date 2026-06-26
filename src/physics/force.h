#pragma once

#include "body.h"
#include "math/vec2.h"

namespace physics::force {

struct SpringParams {
  float rest_length = 1;
  float k = 1;
  float c = 0;
};

math::Vec2 drag(const math::Vec2& velocity, float k);
math::Vec2 friction(const math::Vec2& velocity, float k);
math::Vec2 spring(const Body& a, const Body& b, SpringParams params);
math::Vec2 gravity(float mass, float g = 9.81);

} // namespace physics::force

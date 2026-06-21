#pragma once

#include "math/vec2.h"

namespace physics::force {

math::Vec2 drag(const math::Vec2& velocity, float k);
math::Vec2 friction(const math::Vec2& normal, float k);
math::Vec2 spring(const math::Vec2& position, const math::Vec2& anchor, float rest_length, float k);
math::Vec2 damped_spring(const math::Vec2& position,
                         const math::Vec2& anchor,
                         const math::Vec2& relative_velocity,
                         float rest_length,
                         float c,
                         float k);
math::Vec2 gravity(float mass, float g = 9.81);

} // namespace physics::force

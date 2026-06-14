#pragma once

#include "math/vec2.h"

namespace physics {

math::Vec2 generate_drag_force(const math::Vec2& velocity, float k);

} // namespace physics

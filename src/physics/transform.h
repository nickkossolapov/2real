#pragma once

#include "math/vec2.h"

namespace physics {

struct Transform {
  math::Vec2 position;
  float rotation = 0.0f;
};

} // namespace physics

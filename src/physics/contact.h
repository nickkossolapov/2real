#pragma once

namespace physics {

struct Contact {
  math::Vec2 start;
  math::Vec2 end;

  math::Vec2 normal;
  float depth;
};

} // namespace physics

#pragma once
#include "body.h"

namespace physics {

struct Contact {
  const Body& a; // TODO maybe move these to IDs instead?
  const Body& b;

  math::Vec2 start;
  math::Vec2 end;

  math::Vec2 normal;
  float depth;
};

} // namespace physics

#include "force.h"

namespace physics {

math::Vec2 generate_drag_force(const math::Vec2& velocity, const float k) {
  return -velocity * velocity.length() * k;
}

} // namespace physics
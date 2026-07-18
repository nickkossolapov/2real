#include "world.h"

#include "collision.h"
#include "resolution.h"

namespace physics {

void World::update(const float dt) {
  for (auto& body : bodies_) {
    body.integrate(dt, gravity_);
  }
}

void World::check_collisions() {
  for (int i = 0; i < bodies_.size(); ++i) {
    for (int j = i + 1; j < bodies_.size(); ++j) {
      auto& a = bodies_[i];
      auto& b = bodies_[j];

      if (auto contact = collision::test(a, b)) {
        resolution::resolve(a, b, *contact);
      }
    }
  }
}

} // namespace physics

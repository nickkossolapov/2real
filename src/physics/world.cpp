#include "world.h"

#include "collision.h"
#include "resolution.h"

namespace physics {

void World::update(const float dt) {
  for (auto& body : bodies_) {
    body->update_aabb();
    body->integrate_forces(dt, gravity_);
  }

  for (auto& constraint : joint_constraints_) {
    constraint.pre_solve(dt);
  }

  for (int i = 0; i < 5; ++i) {
    for (auto& constraint : joint_constraints_) {
      constraint.solve();
    }
  }

  for (auto& body : bodies_) {
    body->integrate_positions(dt);
  }

  check_collisions();
}

void World::check_collisions() {
  for (int i = 0; i < bodies_.size(); ++i) {
    for (int j = i + 1; j < bodies_.size(); ++j) {
      auto& a = bodies_[i];
      auto& b = bodies_[j];

      if (!a->aabb().intersects(b->aabb())) {
        continue;
      }

      if (auto contact = collision::test(*a, *b)) {
        resolution::resolve(*a, *b, *contact);
      }
    }
  }
}

} // namespace physics

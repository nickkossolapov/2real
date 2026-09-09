#include "world.h"

#include "collision.h"
#include "constraint/non-penetration.h"
#include "resolution.h"

namespace physics {

void World::update(const float dt) {
  for (auto& body : bodies_) {
    body->update_aabb();
    body->integrate_forces(dt, gravity_);
  }

  std::vector<constraint::NonPenetration> collisions = check_collisions();

  for (auto& collision : collisions) {
    collision.pre_solve(dt);
  }

  for (auto& constraint : joint_constraints_) {
    constraint.pre_solve(dt);
  }

  for (int i = 0; i < 5; ++i) {
    for (auto& collision : collisions) {
      collision.solve();
    }

    for (auto& constraint : joint_constraints_) {
      constraint.solve();
    }
  }

  for (auto& body : bodies_) {
    body->integrate_positions(dt);
  }
}

std::vector<constraint::NonPenetration> World::check_collisions() const {
  std::vector<constraint::NonPenetration> collisions{};

  for (int i = 0; i < bodies_.size(); ++i) {
    for (int j = i + 1; j < bodies_.size(); ++j) {
      auto& a = bodies_[i];
      auto& b = bodies_[j];

      if (!a->aabb().intersects(b->aabb())) {
        continue;
      }

      if (auto contact = collision::test(*a, *b)) {
        collisions.emplace_back(*a, *b, *contact);
      }
    }
  }

  return collisions;
}

} // namespace physics

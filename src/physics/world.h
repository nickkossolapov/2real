#pragma once
#include "body.h"
#include "constraint/joint.h"
#include "constraint/non-penetration.h"

#include <complex.h>
#include <memory>
#include <vector>

namespace physics {

struct World {
  explicit World(const math::Vec2 gravity = {0.0f, -9.81})
      : gravity_(gravity) {}

  std::unique_ptr<Body>& get_body(const int i) { return bodies_[i]; };

  std::vector<std::unique_ptr<Body>>& bodies() { return bodies_; }

  Body* add_body(Body body) {
    auto pointer = std::make_unique<Body>(std::move(body));
    Body* result = pointer.get();

    bodies_.push_back(std::move(pointer));
    return result;
  }

  void add_joint_constraint(constraint::Joint constraint) { joint_constraints_.emplace_back(constraint); }

  void update(float dt);
  std::vector<constraint::NonPenetration> check_collisions() const;

private:
  math::Vec2 gravity_;
  std::vector<std::unique_ptr<Body>> bodies_ = {};
  std::vector<constraint::Joint> joint_constraints_ = {};
};

} // namespace physics
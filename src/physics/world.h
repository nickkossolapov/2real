#pragma once
#include "body.h"
#include "constraint.h"

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

  void add_joint_constraint(JointConstraint constraint) { joint_constraints_.emplace_back(constraint); }

  void update(float dt);
  void check_collisions();

private:
  math::Vec2 gravity_;
  std::vector<std::unique_ptr<Body>> bodies_ = {};
  std::vector<JointConstraint> joint_constraints_ = {};
};

} // namespace physics
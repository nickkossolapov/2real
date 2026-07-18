#pragma once
#include "body.h"

#include <vector>

namespace physics {

struct World {
  explicit World(const math::Vec2 gravity = {0.0f, -9.81})
      : gravity_(gravity) {}

  std::vector<Body>& bodies() { return bodies_; }

  void add_body(const Body& body) { bodies_.push_back(body); }
  void update(float dt);
  void check_collisions();

private:
  math::Vec2 gravity_;
  std::vector<Body> bodies_ = std::vector<Body>();
};

} // namespace physics
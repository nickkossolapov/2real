#pragma once
#include "body.h"

#include <memory>
#include <vector>

namespace physics {

struct World {
  explicit World(const math::Vec2 gravity = {0.0f, -9.81})
      : gravity_(gravity) {}

  std::vector<std::unique_ptr<Body>>& bodies() { return bodies_; }

  Body* add_body(Body body) {
    auto pointer = std::make_unique<Body>(std::move(body));
    Body* result = pointer.get();

    bodies_.push_back(std::move(pointer));
    return result;
  }

  void update(float dt) const;
  void check_collisions() const;

private:
  math::Vec2 gravity_;
  std::vector<std::unique_ptr<Body>> bodies_ = {};
};

} // namespace physics
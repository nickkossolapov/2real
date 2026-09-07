#pragma once
#include "math/mat_mn.h"
#include "physics/body.h"

namespace physics::constraint {

class NonPenetration {
  Body* a_;
  Body* b_;

  float bias_ = 0.0f;

  math::MatMN<1, 6> jacobian_ = {};
  math::VecN<1> cached_lambda_ = {};

  math::MatMN<6, 6> get_inv_m() const;

  math::VecN<6> get_velocities() const;

public:
  explicit NonPenetration(Body& a, Body& b)
      : a_(&a),
        b_(&b) {}

  void pre_solve(float dt);
  void solve();
};
} // namespace physics::constraint

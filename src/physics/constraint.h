#pragma once
#include "body.h"
#include "math/mat_mn.h"

namespace physics {

struct JointConstraint {
private:
  Body* a_;
  Body* b_;
  math::Vec2 anchor_a_local_;
  math::Vec2 anchor_b_local_;

  math::MatMN<1, 6> jacobian_ = {};

  math::MatMN<6, 6> get_inv_m() const;
  math::VecN<6> get_velocities() const;

public:
  JointConstraint(Body& a, Body& b, const math::Vec2 anchor)
      : a_(&a),
        b_(&b),
        anchor_a_local_(a.world_to_local_point(anchor)),
        anchor_b_local_(b.world_to_local_point(anchor)) {}

  void solve();
};

struct ContactConstraint {
  Body* a;
  Body* b;

  math::MatMN<6, 6> get_inv_m() const;
  math::VecN<6> get_velocities() const;

  void solve();
};

} // namespace physics

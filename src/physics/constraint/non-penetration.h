#pragma once
#include "math/mat_mn.h"
#include "physics/body.h"
#include "physics/contact.h"

namespace physics::constraint {

class NonPenetration {
  Body* a_;
  Body* b_;
  math::Vec2 point_a_local_;
  math::Vec2 point_b_local_;
  math::Vec2 normal_a_local_;

  float bias_ = 0.0f;

  math::MatMN<1, 6> jacobian_ = {};
  math::VecN<1> cached_lambda_ = {};

  math::MatMN<6, 6> get_inv_m() const;

  math::VecN<6> get_velocities() const;

public:
  explicit NonPenetration(Body& a, Body& b, const Contact& contact)
      : a_(&a),
        b_(&b),
        point_a_local_(a.world_to_local_point(contact.start)),
        point_b_local_(b.world_to_local_point(contact.end)),
        normal_a_local_(a.world_to_local_vector(contact.normal)) {}

  void pre_solve(float dt);
  void solve();
};

} // namespace physics::constraint

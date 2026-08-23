#include "constraint.h"

#include "math_utils.h"

namespace physics {

math::MatMN<6, 6> JointConstraint::get_inv_m() const {
  math::MatMN<6, 6> inv_m{};

  inv_m(0, 0) = a_->inv_mass();
  inv_m(1, 1) = a_->inv_mass();
  inv_m(2, 2) = a_->inv_inertia();
  inv_m(3, 3) = b_->inv_mass();
  inv_m(4, 4) = b_->inv_mass();
  inv_m(5, 5) = b_->inv_inertia();

  return inv_m;
}

math::VecN<6> JointConstraint::get_velocities() const {
  return math::VecN<6>{
      a_->velocity.x,
      a_->velocity.y,
      a_->angular_velocity,
      b_->velocity.x,
      b_->velocity.y,
      b_->angular_velocity,
  };
}

void JointConstraint::solve() {
  const math::Vec2 d = a_->world_to_local_point(anchor_a_local_) - b_->world_to_local_point(anchor_b_local_);

  const auto jacobian = math::MatMN<6, 1>{
      2.0f * d.x,
      2.0f * d.y,
      2.0f * math_utils::cross(anchor_a_local_, d),
      2.0f * -d.x,
      2.0f * -d.y,
      2.0f * math_utils::cross(anchor_b_local_, -d),
  };

  const math::VecN<6> v = get_velocities();
  const math::MatMN<6, 6> inv_m = get_inv_m();
}

} // namespace physics
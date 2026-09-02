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

void JointConstraint::pre_solve(const float dt) {
  const math::Vec2 anchor_a = a_->local_to_world_point(anchor_a_local_);
  const math::Vec2 anchor_b = b_->local_to_world_point(anchor_b_local_);
  const math::Vec2 d = anchor_a - anchor_b;

  jacobian_ = math::MatMN<1, 6>{
      2.0f * d.x,
      2.0f * d.y,
      2.0f * math_utils::cross(d, anchor_a - a_->position),
      2.0f * -d.x,
      2.0f * -d.y,
      2.0f * math_utils::cross(-d, anchor_b - b_->position),
  };

  // warm starting
  const auto impulses = jacobian_.transpose() * cached_lambda_;

  a_->add_impulse_linear(math::Vec2{.x = impulses[0], .y = impulses[1]});
  a_->add_impulse_angular(impulses[2]);
  b_->add_impulse_linear(math::Vec2{.x = impulses[3], .y = impulses[4]});
  b_->add_impulse_angular(impulses[5]);

  // Baumgarte stabilization with positional error
  constexpr float beta = 0.1f;
  float c = math::dot(d, d);
  c = std::max(0.0f, c - 0.01f);

  bias_ = beta / dt * c;
}

void JointConstraint::solve() {
  const auto j = jacobian_;
  const auto j_t = j.transpose();

  const math::VecN<6> v = get_velocities();
  const math::MatMN<6, 6> inv_m = get_inv_m();

  const auto lhs = j * inv_m * j.transpose();
  auto rhs = j * v * -1.0f;
  rhs[0] -= bias_;

  const auto lamba = math::solve_linear_system(lhs, rhs);

  cached_lambda_ += lamba;

  const auto impulses = j_t * lamba;
  a_->add_impulse_linear(math::Vec2{.x = impulses[0], .y = impulses[1]});
  a_->add_impulse_angular(impulses[2]);
  b_->add_impulse_linear(math::Vec2{.x = impulses[3], .y = impulses[4]});
  b_->add_impulse_angular(impulses[5]);
}

} // namespace physics
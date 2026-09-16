#include "non-penetration.h"

#include "physics/math_utils.h"

namespace physics::constraint {

math::MatMN<6, 6> NonPenetration::get_inv_m() const {
  math::MatMN<6, 6> inv_m{};

  inv_m(0, 0) = a_->inv_mass();
  inv_m(1, 1) = a_->inv_mass();
  inv_m(2, 2) = a_->inv_inertia();
  inv_m(3, 3) = b_->inv_mass();
  inv_m(4, 4) = b_->inv_mass();
  inv_m(5, 5) = b_->inv_inertia();

  return inv_m;
}

math::VecN<6> NonPenetration::get_velocities() const {
  return math::VecN<6>{
      a_->velocity.x,
      a_->velocity.y,
      a_->angular_velocity,
      b_->velocity.x,
      b_->velocity.y,
      b_->angular_velocity,
  };
}

void NonPenetration::pre_solve(const float dt) {
  const math::Vec2 p_a = a_->local_to_world_point(point_a_local_);
  const math::Vec2 p_b = b_->local_to_world_point(point_b_local_);
  const math::Vec2 n = a_->local_to_world_vector(normal_a_local_);

  const math::Vec2 r_a = p_a - a_->position;
  const math::Vec2 r_b = p_b - b_->position;

  jacobian_ = math::MatMN<2, 6>{};

  jacobian_(0, 0) = -n.x;
  jacobian_(0, 1) = -n.y;
  jacobian_(0, 2) = -math_utils::cross(n, p_a - a_->position);
  jacobian_(0, 3) = n.x;
  jacobian_(0, 4) = n.y;
  jacobian_(0, 5) = math_utils::cross(n, p_b - b_->position);

  if (friction_ > 0.0f) {
    const math::Vec2 t = math_utils::right_perpendicular(n);

    jacobian_(1, 0) = -t.x;
    jacobian_(1, 1) = -t.y;
    jacobian_(1, 2) = -math_utils::cross(t, p_a - a_->position);
    jacobian_(1, 3) = t.x;
    jacobian_(1, 4) = t.y;
    jacobian_(1, 5) = math_utils::cross(t, p_b - b_->position);
  }

  // warm starting
  const auto impulses = jacobian_.transpose() * cached_lambda_;

  a_->add_impulse_linear(math::Vec2{.x = impulses[0], .y = impulses[1]});
  a_->add_impulse_angular(impulses[2]);
  b_->add_impulse_linear(math::Vec2{.x = impulses[3], .y = impulses[4]});
  b_->add_impulse_angular(impulses[5]);

  // Baumgarte stabilization with positional error
  constexpr float beta = 0.1f;
  float c = math::dot(p_b - p_a, n);
  c = std::min(0.0f, c + 0.01f);

  // collision elasticity
  const math::Vec2 va = a_->velocity + math_utils::right_perpendicular(r_a) * a_->angular_velocity;
  const math::Vec2 vb = b_->velocity + math_utils::right_perpendicular(r_b) * b_->angular_velocity;
  const float v_rel_normal = math::dot(va - vb, n);

  const float e = std::min(a_->restitution, b_->restitution);

  const float restitution_bias = v_rel_normal > 0.0f ? -e * v_rel_normal : 0.0f;

  bias_ = beta / dt * c + restitution_bias;
}

void NonPenetration::solve() {
  const auto j = jacobian_;
  const auto j_t = j.transpose();

  const math::VecN<6> v = get_velocities();
  const math::MatMN<6, 6> inv_m = get_inv_m();

  const auto lhs = j * inv_m * j.transpose();
  auto rhs = j * v * -1.0f;
  rhs[0] -= bias_;

  auto lamba = math::solve_linear_system(lhs, rhs);

  // cached_lambda_ += lamba;
  const math::VecN prev_lambda = cached_lambda_;
  cached_lambda_ += lamba;
  cached_lambda_[0] = cached_lambda_[0] < 0.0f ? 0.0f : cached_lambda_[0];

  if (friction_ > 0.0f) {
    const float max_friction = cached_lambda_[0] * friction_;
    cached_lambda_[1] = std::clamp(cached_lambda_[1], -max_friction, max_friction);
  }

  lamba = cached_lambda_ - prev_lambda;

  const auto impulses = j_t * lamba;
  a_->add_impulse_linear(math::Vec2{.x = impulses[0], .y = impulses[1]});
  a_->add_impulse_angular(impulses[2]);
  b_->add_impulse_linear(math::Vec2{.x = impulses[3], .y = impulses[4]});
  b_->add_impulse_angular(impulses[5]);
}

} // namespace physics::constraint
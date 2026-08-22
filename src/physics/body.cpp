#include "body.h"

#include "math_utils.h"

namespace physics {

namespace {

math::Rect calculate_aabb(const math::Vec2 position, const float rotation, Shape shape) {
  if (std::holds_alternative<shape::Circle>(shape)) {
    const auto [radius] = std::get<shape::Circle>(shape);

    return {
        .x_min = position.x - radius,
        .x_max = position.x + radius,
        .y_min = position.y - radius,
        .y_max = position.y + radius,
    };
  }

  if (std::holds_alternative<shape::Polygon>(shape)) {
    const auto [local_points] = std::get<shape::Polygon>(shape);
    const std::vector<math::Vec2> points = math_utils::to_world(position, local_points, rotation);

    math::Rect rect = {
        .x_min = points[0].x,
        .x_max = points[0].x,
        .y_min = points[0].y,
        .y_max = points[0].y,
    };

    for (auto [x, y] : points) {
      rect.x_min = std::min(rect.x_min, x);
      rect.x_max = std::max(rect.x_max, x);
      rect.y_min = std::min(rect.y_min, y);
      rect.y_max = std::max(rect.y_max, y);
    }

    return rect;
  }

  return {};
}

} // namespace

Body::Body(const float m, const Shape& shape, const math::Vec2 pos, const float rot)
    : Body(m, 0.0f, shape, pos, rot) {}

Body::Body(const float m, const float restitution, const Shape& shape, const math::Vec2 pos, const float rot)
    : Body(m, restitution, 0.2f, shape, pos, rot) {}

Body::Body(const float m,
           const float restitution,
           const float friction,
           const Shape& shape,
           const math::Vec2 pos,
           const float rot)
    : position(pos),
      rotation(rot),
      mass_(m > 0.0f ? m : 0.0f),
      inv_mass_(m > 0.0f ? 1.0f / m : 0.0f),
      shape_(shape),
      inertia_(compute_moment_of_inertia(shape, mass_)),
      inv_inertia_(inertia_ > 0.0f ? 1.0f / inertia_ : 0.0f),
      restitution(restitution),
      friction(friction),
      aabb_(calculate_aabb(position, rotation, shape)) {}

void Body::integrate(const float dt, const math::Vec2 gravity) {
  if (is_static()) {
    return;
  }

  const math::Vec2 acceleration = gravity * gravity_scale + net_force_ * inv_mass_;

  velocity += acceleration * dt;
  position += velocity * dt;

  const float angular_acceleration = net_torque_ * inv_inertia_;

  angular_velocity += angular_acceleration * dt;
  rotation += angular_velocity * dt;

  reset();
}

void Body::add_force(const math::Vec2 force) {
  net_force_ += force;
}

void Body::add_torque(const float torque) {
  net_torque_ += torque;
}

void Body::add_impulse(const math::Vec2 j) {
  if (is_static()) {
    return;
  }

  velocity += j * inv_mass_;
}

void Body::add_impulse(const math::Vec2 j, const math::Vec2 r) {
  if (is_static()) {
    return;
  }

  velocity += j * inv_mass_;
  angular_velocity += math_utils::cross(j, r) * inv_inertia_;
}

void Body::update_aabb() {
  aabb_ = calculate_aabb(position, rotation, shape_);
}

math::Vec2 Body::world_to_local_point(const math::Vec2& point) const {
  const math::Vec2 local_space = point - position;

  return {
      .x = std::cos(-rotation) * local_space.x + std::sin(-rotation) * local_space.y,
      .y = -std::sin(-rotation) * local_space.x + std::cos(-rotation) * local_space.y,
  };
}

void Body::reset() {
  net_force_ = {};
  net_torque_ = 0;
}

} // namespace physics
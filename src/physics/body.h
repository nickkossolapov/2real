#pragma once

#include "math/rect.h"
#include "math/vec2.h"
#include "shape.h"

#include <variant>

namespace physics {

struct Body {
  math::Vec2 position;
  math::Vec2 velocity;

  float gravity_scale = 1.0f;

  float rotation = 0;
  float angular_velocity = 0;

  float restitution;
  float friction;

  explicit Body(float m, const Shape& shape, math::Vec2 pos = {}, float rot = 0);
  explicit Body(float m, float restitution, const Shape& shape, math::Vec2 pos = {}, float rot = 0);
  explicit Body(float m, float restitution, float friction, const Shape& shape, math::Vec2 pos = {}, float rot = 0);

  void integrate(float dt, math::Vec2 gravity);
  void add_force(math::Vec2 force);
  void add_torque(float torque);
  void add_impulse(math::Vec2 j);
  void add_impulse(math::Vec2 j, math::Vec2 r);

  math::Rect aabb() const { return aabb_; }
  void update_aabb();

  bool is_static() const { return inv_mass_ < math::epsilon; };

  float inv_mass() const { return inv_mass_; }
  float inv_inertia() const { return inv_inertia_; }
  Shape shape() const { return shape_; }

private:
  math::Vec2 net_force_;
  float net_torque_ = 0.0f;

  float mass_;
  float inv_mass_;
  float inertia_;
  float inv_inertia_;

  Shape shape_;
  math::Rect aabb_;

  void reset();
};

} // namespace physics

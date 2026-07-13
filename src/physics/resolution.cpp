#include "resolution.h"

#include <cmath>

namespace physics::resolution {

namespace {

void resolve_penetration(Body& a, Body& b, const Contact& contact) {
  const float da = contact.depth / (a.inv_mass + b.inv_mass) * a.inv_mass;
  const float db = contact.depth / (a.inv_mass + b.inv_mass) * b.inv_mass;

  a.position -= contact.normal * da;
  b.position += contact.normal * db;
}

void resolve_impulse(Body& a, Body& b, const Contact& contact) {
  const float e = std::min(a.restitution, b.restitution);

  const math::Vec2 ra = contact.end - a.position;
  const math::Vec2 va = a.velocity + ra.perpendicular() * a.angular_velocity;
  const math::Vec2 rb = contact.start - b.position;
  const math::Vec2 vb = b.velocity + rb.perpendicular() * b.angular_velocity;

  const math::Vec2 v_rel = va - vb;

  const float denom_linear = a.inv_mass + b.inv_mass;
  const float denom_angular = std::pow(math::cross(ra, contact.normal), 2) * a.inv_inertia +
                              std::pow(math::cross(rb, contact.normal), 2) * b.inv_inertia;

  const float impulse = -(1 + e) * math::dot(v_rel, contact.normal) / (denom_linear + denom_angular);
  const math::Vec2 jn = contact.normal * impulse;

  a.add_impulse(jn, ra);
  b.add_impulse(-jn, rb);
}

} // namespace

void resolve(Body& a, Body& b, const Contact& contact) {
  if (a.is_static() && b.is_static()) {
    return;
  }

  resolve_penetration(a, b, contact);
  resolve_impulse(a, b, contact);
}

} // namespace physics::resolution
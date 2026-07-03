#include "resolution.h"

namespace physics::resolution {

namespace {

void project(Body& a, Body& b, const Contact& contact) {
  const float da = contact.depth / (a.inv_mass + b.inv_mass) * a.inv_mass;
  const float db = contact.depth / (a.inv_mass + b.inv_mass) * b.inv_mass;

  a.position -= contact.normal * da;
  b.position += contact.normal * db;
}

void resolve_impulse(Body& a, Body& b, const Contact& contact) {
  const float e = std::min(a.restitution, b.restitution);
  const math::Vec2 v_rel = a.velocity - b.velocity;

  const float impulse = -(1 + e) * math::dot(v_rel, contact.normal) / (a.inv_mass + b.inv_mass);
  const math::Vec2 jn = contact.normal * impulse;

  a.add_impulse(jn);
  b.add_impulse(-jn);
}

} // namespace

void resolve(Body& a, Body& b, const Contact& contact) {
  if (a.is_static() && b.is_static()) {
    return;
  }

  project(a, b, contact);
  resolve_impulse(a, b, contact);
}

} // namespace physics::resolution
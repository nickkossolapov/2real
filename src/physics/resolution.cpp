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
  const math::Vec2 ra = contact.end - a.position;
  const math::Vec2 va = a.velocity + ra.right_perpendicular() * a.angular_velocity;
  const math::Vec2 rb = contact.start - b.position;
  const math::Vec2 vb = b.velocity + rb.right_perpendicular() * b.angular_velocity;

  const math::Vec2 v_rel = va - vb;

  // TODO move to contact manifold, and only apply normal impulse if the speed is about a certain speed?
  const float e = std::min(a.restitution, b.restitution);
  const float normal_impulse = -(1 + e) * math::dot(v_rel, contact.normal) /
                               (a.inv_mass + b.inv_mass + std::pow(math::cross(ra, contact.normal), 2) * a.inv_inertia +
                                std::pow(math::cross(rb, contact.normal), 2) * b.inv_inertia);
  const math::Vec2 jn = contact.normal * normal_impulse;

  const math::Vec2 tangent = contact.normal.right_perpendicular();

  const float f = std::max(a.friction, b.friction);
  const float tangential_impulse = -(1 + f) * math::dot(v_rel, tangent) /
                                   (a.inv_mass + b.inv_mass + std::pow(math::cross(ra, tangent), 2) * a.inv_inertia +
                                    std::pow(math::cross(rb, tangent), 2) * b.inv_inertia);
  const math::Vec2 jt = tangent * tangential_impulse;

  const math::Vec2 j = jn + jt;

  a.add_impulse(j, ra);
  b.add_impulse(-j, rb);
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
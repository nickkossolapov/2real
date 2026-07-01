#include "collision.h"

#include "transform.h"

namespace physics::collision {

namespace {

std::optional<Contact> test_circle_circle(const Body& a, const Body& b) {
  const auto [a_radius] = std::get<shape::Circle>(a.shape);
  const auto [b_radius] = std::get<shape::Circle>(b.shape);

  const math::Vec2 ab = b.position - a.position;
  const float radius_sum = a_radius + b_radius;

  if (ab.length_squared() > radius_sum * radius_sum) {
    return {};
  }

  Contact contact{
      .a = a,
      .b = b,
  };

  contact.normal = ab.normalized();
  contact.start = b.position - contact.normal * b_radius;
  contact.end = a.position + contact.normal * a_radius;
  contact.depth = (contact.end - contact.start).length();

  return contact;
}

std::optional<Contact> test(const shape::Circle& a, const Transform a_t, const shape::Box& b, const Transform b_t) {
  return {};
}

std::optional<Contact> test(const shape::Box& a, const Transform a_t, const shape::Box& b, const Transform b_t) {
  return {};
}

} // namespace

std::optional<Contact> test(const Body& a, const Body& b) {
  auto visitor = Overloaded{
      [&](const shape::Circle&, const shape::Circle&) { return test_circle_circle(a, b); },
      [&](const shape::Circle& a_c, const shape::Box& b_box) {
        return test(a_c, {a.position}, b_box, {b.position, b.rotation});
      },
      [&](const shape::Box& a_box, const shape::Circle& b_circle) {
        return test(b_circle, {b.position, b.rotation}, a_box, {a.position});
      },
      [&](const shape::Box& a_box, const shape::Box& b_box) {
        return test(a_box, {a.position, a.rotation}, b_box, {b.position, b.rotation});
      },
  };

  return std::visit(visitor, a.shape, b.shape);
}

} // namespace physics::collision
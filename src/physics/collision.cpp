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

  Contact contact;

  contact.normal = ab.normalized();
  contact.start = b.position - contact.normal * b_radius;
  contact.end = a.position + contact.normal * a_radius;
  contact.depth = (contact.end - contact.start).length();

  return contact;
}

std::vector<math::Vec2> to_world(const math::Vec2 position, const std::vector<math::Vec2>& local, const float a) {
  std::vector<math::Vec2> world_points;

  for (int i = 0; i < local.size(); ++i) {
    const auto [x, y] = local[i];

    // left-handed rotation matrix
    const float world_x = std::cos(a) * x + std::sin(a) * y + position.x;
    const float world_y = -std::sin(a) * x + std::cos(a) * y + position.y;

    world_points.emplace_back(world_x, world_y);
  }

  return world_points;
}

float find_min_separation(const std::vector<math::Vec2>& a, const std::vector<math::Vec2>& b) {
  float separation = std::numeric_limits<float>::lowest();

  for (int i = 0; i < a.size(); ++i) {
    const int next = (i + 1) % a.size();
    math::Vec2 edge = (a[next] - a[i]).perpendicular();

    float min_sep = std::numeric_limits<float>::max();

    for (int j = 0; j < b.size(); ++j) {
      float projection = dot(b[j] - a[i], edge);
      min_sep = std::min(projection, min_sep);
    }

    separation = std::max(separation, min_sep);
  }

  return separation;
}

std::optional<Contact> test_polygon_polygon(const Body& a, const Body& b) {
  const auto& a_shape = std::get<shape::Polygon>(a.shape);
  const auto& b_shape = std::get<shape::Polygon>(b.shape);

  const auto a_points = to_world(a.position, a_shape.points, a.rotation);
  const auto b_points = to_world(b.position, b_shape.points, b.rotation);

  if (find_min_separation(a_points, b_points) >= 0 || find_min_separation(b_points, a_points) >= 0) {
    return {};
  }

  return Contact{
      {0, 0},
      {0, 0},
      {0, 0},
      0,
  };
}

std::optional<Contact> test(const shape::Circle& a, const Transform a_t, const shape::Polygon& b, const Transform b_t) {
  return {};
}

std::optional<Contact> test(const shape::Polygon& a,
                            const Transform a_t,
                            const shape::Polygon& b,
                            const Transform b_t) {
  return {};
}

} // namespace

std::optional<Contact> test(const Body& a, const Body& b) {
  auto visitor = Overloaded{
      [&](const shape::Circle&, const shape::Circle&) { return test_circle_circle(a, b); },
      [&](const shape::Circle& a_c, const shape::Polygon& b_box) {
        return test(a_c, {a.position}, b_box, {b.position, b.rotation});
      },
      [&](const shape::Polygon& a_box, const shape::Circle& b_circle) {
        return test(b_circle, {b.position, b.rotation}, a_box, {a.position});
      },
      [&](const shape::Polygon&, const shape::Polygon&) { return test_polygon_polygon(a, b); },
  };

  return std::visit(visitor, a.shape, b.shape);
}

} // namespace physics::collision
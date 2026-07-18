#include "collision.h"

#include "math_utils.h"

namespace physics::collision {

namespace {

std::optional<Contact> test(const Body& a, const shape::Circle& a_shape, const Body& b, const shape::Circle& b_shape) {

  const math::Vec2 ab = b.position - a.position;
  const float radius_sum = a_shape.radius + b_shape.radius;

  if (ab.length_squared() > radius_sum * radius_sum) {
    return {};
  }

  Contact contact;

  contact.normal = ab.normalized();
  contact.start = b.position - contact.normal * b_shape.radius;
  contact.end = a.position + contact.normal * a_shape.radius;
  contact.depth = (contact.end - contact.start).length();

  return contact;
}

struct PolygonSeparation {
  float distance;
  math::Vec2 axis; // unit outward normal of the reference face (points out of polygon `a`)
  math::Vec2 point;
};

PolygonSeparation find_min_separation(const std::vector<math::Vec2>& a, const std::vector<math::Vec2>& b) {
  PolygonSeparation separation = {.distance = std::numeric_limits<float>::lowest()};

  for (int i = 0; i < a.size(); ++i) {
    const int next = (i + 1) % a.size();
    const math::Vec2 edge_normal = math_utils::normal(a[next] - a[i]);

    float min_sep = std::numeric_limits<float>::max();
    math::Vec2 min_vertex;

    for (int j = 0; j < b.size(); ++j) {
      const float projection = dot(b[j] - a[i], edge_normal);

      if (projection < min_sep) {
        min_sep = projection;
        min_vertex = b[j];
      }
    }

    if (min_sep > separation.distance) {
      separation.distance = min_sep;
      separation.axis = edge_normal;
      separation.point = min_vertex;
    }
  }

  return separation;
}

std::optional<Contact> test(const Body& a,
                            const shape::Polygon& a_shape,
                            const Body& b,
                            const shape::Polygon& b_shape) {
  const auto a_points = math_utils::to_world(a.position, a_shape.points, a.rotation);
  const auto b_points = math_utils::to_world(b.position, b_shape.points, b.rotation);

  const auto ab_separation = find_min_separation(a_points, b_points);

  if (ab_separation.distance >= 0) {
    return {};
  }

  const auto ba_separation = find_min_separation(b_points, a_points);

  if (ba_separation.distance >= 0) {
    return {};
  }

  if (ab_separation.distance > ba_separation.distance) {
    return Contact{
        .start = ab_separation.point,
        .end = ab_separation.point - ab_separation.axis * ab_separation.distance,
        .normal = ab_separation.axis,
        .depth = -ab_separation.distance,
    };
  }

  return Contact{
      .start = ba_separation.point - ba_separation.axis * ba_separation.distance,
      .end = ba_separation.point,
      .normal = -ba_separation.axis,
      .depth = -ba_separation.distance,
  };
}

std::optional<Contact> test(const Body& polygon,
                            const shape::Polygon& polygon_shape,
                            const Body& circle,
                            const shape::Circle& circle_shape) {
  const auto polygon_points = math_utils::to_world(polygon.position, polygon_shape.points, polygon.rotation);

  int vertex = 0;
  float max_projection = std::numeric_limits<float>::lowest();

  for (int i = 0; i < polygon_points.size(); ++i) {
    const int next = (i + 1) % polygon_points.size();
    const math::Vec2 edge_normal = math_utils::normal(polygon_points[next] - polygon_points[i]);
    const float projection = math::dot(circle.position - polygon_points[i], edge_normal);

    if (projection > max_projection) {
      vertex = i;
      max_projection = projection;
    }
  }

  // Check region: nearest left, right, or edge
  const int end = (vertex + 1) % polygon_points.size();
  const math::Vec2 edge = polygon_points[end] - polygon_points[vertex];
  const float radius_sq = circle_shape.radius * circle_shape.radius;

  if (math::dot(circle.position - polygon_points[vertex], edge) < 0) {
    if ((circle.position - polygon_points[vertex]).length_squared() < radius_sq) {
      const math::Vec2 normal = (circle.position - polygon_points[vertex]).normalized();
      const float depth = circle_shape.radius - (circle.position - polygon_points[vertex]).length();

      return Contact{
          .start = circle.position - normal * circle_shape.radius,
          .end = polygon_points[vertex],
          .normal = normal,
          .depth = depth,
      };
    }

    return {};
  }

  if (math::dot(circle.position - polygon_points[end], -edge) < 0) {
    if ((circle.position - polygon_points[end]).length_squared() < radius_sq) {
      const math::Vec2 normal = (circle.position - polygon_points[end]).normalized();
      const float depth = circle_shape.radius - (circle.position - polygon_points[end]).length();

      return Contact{
          .start = circle.position - normal * circle_shape.radius,
          .end = polygon_points[end],
          .normal = normal,
          .depth = depth,
      };
    }

    return {};
  }

  if (max_projection > circle_shape.radius) {
    return {};
  }

  const math::Vec2 normal = math_utils::normal(polygon_points[end] - polygon_points[vertex]);
  const float depth = circle_shape.radius - max_projection;

  return Contact{
      .start = circle.position - normal * circle_shape.radius,
      .end = circle.position - normal * (circle_shape.radius - depth),
      .normal = normal,
      .depth = depth,
  };
}

std::optional<Contact> test(const Body& circle,
                            const shape::Circle& circle_shape,
                            const Body& polygon,
                            const shape::Polygon& polygon_shape) {
  const auto contact = test(polygon, polygon_shape, circle, circle_shape);

  if (!contact) {
    return {};
  }

  return contact->flipped();
}

} // namespace

std::optional<Contact> test(const Body& a, const Body& b) {
  if (a.is_static() && b.is_static()) {
    return {};
  }

  return std::visit([&](const auto& a_s, const auto& b_s) { return test(a, a_s, b, b_s); }, a.shape(), b.shape());
}

} // namespace physics::collision
#include "drawer.h"
#include "engine/run.h"
#include "engine/sdl.h"
#include "input/input.h"
#include "math/rect.h"
#include "physics/body.h"
#include "physics/collision.h"
#include "physics/force.h"
#include "physics/resolution.h"
#include "render/color.h"
#include "render/framebuffer.h"

#include <set>

namespace {

math::Vec2 to_world(const math::Vec2 center, const math::Vec2 local, const float a) {
  // left-handed rotation matrix
  const float x = std::cos(a) * local.x + std::sin(a) * local.y;
  const float y = -std::sin(a) * local.x + std::cos(a) * local.y;

  return center + math::Vec2{x, y};
}

std::vector<math::Vec2> to_world2(const math::Vec2 position, const std::vector<math::Vec2>& local, const float a) {
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

std::tuple<math::Vec2, math::Vec2> test_circle_polygon_nearest_vertex(const physics::Body& circle,
                                                                      const physics::Body& polygon) {
  const auto& polygon_shape = std::get<physics::shape::Polygon>(polygon.shape);
  const auto polygon_points = to_world2(polygon.position, polygon_shape.points, polygon.rotation);

  int nearest_point = 0;
  float nearest_distance_sq = std::numeric_limits<float>().max();

  for (int i = 0; i < polygon_points.size(); ++i) {
    const float distance_sq = (circle.position - polygon_points[i]).length_squared();

    if (distance_sq < nearest_distance_sq) {
      nearest_point = i;
      nearest_distance_sq = distance_sq;
    }
  }

  const math::Vec2 circle_point = circle.position - polygon_points[nearest_point];
  // Check right and left projections to determine the nearest edge
  const int next = nearest_point == polygon_points.size() - 1 ? 0 : nearest_point + 1;
  const math::Vec2 right_edge = (polygon_points[nearest_point] - polygon_points[next]).normalized();
  const float right_projection = math::dot(circle_point, right_edge);

  const int prev = nearest_point == 0 ? polygon_points.size() - 1 : nearest_point - 1;
  const math::Vec2 left_edge = (polygon_points[nearest_point] - polygon_points[prev]).normalized();
  const float left_projection = math::dot(circle_point, left_edge);

  if (right_projection < left_projection) {
    return {polygon_points[nearest_point], polygon_points[next]};
  }

  return {polygon_points[prev], polygon_points[nearest_point]};
}

std::tuple<math::Vec2, math::Vec2> test_circle_polygon_sat(const physics::Body& circle, const physics::Body& polygon) {
  const auto& polygon_shape = std::get<physics::shape::Polygon>(polygon.shape);
  const auto polygon_points = to_world2(polygon.position, polygon_shape.points, polygon.rotation);

  int nearest_point = 0;
  float closest_projection = std::numeric_limits<float>().min();

  for (int i = 0; i < polygon_points.size(); ++i) {
    const math::Vec2 circle_point = circle.position - polygon_points[i];
    const int next = (i + 1) % polygon_points.size();
    const math::Vec2 edge_normal = (polygon_points[next] - polygon_points[i]).perpendicular().normalized();

    const float projection = math::dot(circle_point, edge_normal);

    if (projection < 0 && projection > closest_projection) {
      nearest_point = i;
      closest_projection = projection;
    } else if (projection < closest_projection) {
      nearest_point = i;
      closest_projection = projection;
    }
  }

  const int next = (nearest_point + 1) % polygon_points.size();

  return {polygon_points[nearest_point], polygon_points[next]};
}

void render_body(const Drawer& drawer, render::Framebuffer& fb, const physics::Body& body, uint32_t color) {
  auto visitor = Overloaded{
      [&](const physics::shape::Circle& c) { drawer.debug_circle(fb, body.position, c.radius, body.rotation, color); },
      [&](const physics::shape::Polygon& p) {
        const int count = p.points.capacity();

        for (int i = 0; i < count; ++i) {
          const int next = (i + 1) % count;

          drawer.line(fb,
                      to_world(body.position, p.points[i], body.rotation),
                      to_world(body.position, p.points[next], body.rotation),
                      color);
        }
      },
  };

  return std::visit(visitor, body.shape);
}

} // namespace

int main(int argc, char* argv[]) {
  constexpr engine::SdlSettings settings{
      .width = 1000,
      .height = 800,
      .scale = 1,
      .enable_v_sync = true,
  };

  constexpr engine::AppConfig app_config = {
      .sdl_settings = settings,
      .background = render::color::near_black,
  };

  constexpr float pixels_per_meter = 20.0f;

  Drawer drawer(pixels_per_meter);

  constexpr float world_width = settings.width / pixels_per_meter;
  constexpr float world_height = settings.height / pixels_per_meter;

  bool is_holding = false;
  int held_particle = 0;
  math::Vec2 pointer;

  std::vector<physics::Body> bodies{};

  bodies.emplace_back(0.0f,
                      0.5f,
                      physics::shape::Polygon({
                          // {0.0f, 10.0f},
                          {10.0f, 0.0f},
                          {5.0f, -5.0f},
                          {-5.0f, -5.0f},
                          {-10.0f, 0.0f},
                      }),
                      math::Vec2{30.0f, 18.0f});
  bodies.emplace_back(0.0f, 0.5f, physics::shape::Circle(3.0f), math::Vec2{15.0f, 18.0f});

  bodies[0].rotation = 1.4f;

  auto update = [&bodies](const float dt, const input::InputState& input) {
    for (auto& body : bodies) {
      body.add_force(physics::force::gravity(body.mass));
    }

    for (auto& body : bodies) {
      body.integrate(dt);
    }

    for (int i = 0; i < bodies.size(); ++i) {
      for (int j = i + 1; j < bodies.size(); ++j) {
        auto& a = bodies[i];
        auto& b = bodies[j];

        // if (auto c = physics::collision::test(a, b)) {
        //   physics::resolution::resolve(a, b, *c);
        // }
      }
    }
  };

  auto read_input = [&bodies, &pointer, &is_holding, &held_particle](const input::InputState& state,
                                                                     const input::InputEvents& events) {
    pointer = {.x = state.cursor_position.x / pixels_per_meter,
               .y = (settings.height - state.cursor_position.y) / pixels_per_meter};

    bodies[1].position = pointer;

    // if (events.primary == input::Event::Released) {
    //   bodies.emplace_back(1.0f, 0.8f, physics::shape::Circle(1.0f), pointer);
    // }
    //
    // if (events.secondary == input::Event::Released) {
    //   bodies.emplace_back(1.0f, 0.8f, physics::shape::box(2.0f, 2.0f), pointer);
    // }
  };

  auto render = [&bodies, &is_holding, &held_particle, &pointer, &drawer](render::Framebuffer& fb) {
    if (is_holding) {
      drawer.line(fb, bodies[held_particle].position, pointer, render::color::red);
    }

    for (auto& body : bodies) {
      render_body(drawer, fb, body, render::color::white);
    }

    const auto circle = bodies[1];
    const auto polygon = bodies[0];

    const auto& circle_shape = std::get<physics::shape::Circle>(circle.shape);

    auto [start, end] = test_circle_polygon_sat(circle, polygon);

    drawer.line(fb, start, end, render::color::red);
    drawer.line(fb, start, pointer, render::color::purple);
  };

  return engine::run(app_config, read_input, update, render);
}

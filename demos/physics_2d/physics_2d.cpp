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
std::tuple<math::Vec2, math::Vec2, bool> test_circle_polygon_vertex_and_edge(const physics::Body& circle,
                                                                             const physics::Body& polygon) {
  const auto& circle_shape = std::get<physics::shape::Circle>(circle.shape);
  const auto& polygon_shape = std::get<physics::shape::Polygon>(polygon.shape);
  const auto polygon_points = to_world2(polygon.position, polygon_shape.points, polygon.rotation);

  bool is_inside = true;

  for (int i = 0; i < polygon_points.size(); ++i) {
    const int next = (i + 1) % polygon_points.size();
    const math::Vec2 edge_normal = (polygon_points[next] - polygon_points[i]).normal();

    if (math::dot(circle.position - polygon_points[i], edge_normal) > 0) {
      is_inside = false;
      break;
    }
  }

  // Check vertices
  int nearest_vertex = 0;
  float nearest_distance_sq = std::numeric_limits<float>().max();

  for (int i = 0; i < polygon_points.size(); ++i) {
    const float distance_sq = (circle.position - polygon_points[i]).length_squared();

    if (distance_sq < nearest_distance_sq) {
      nearest_vertex = i;
      nearest_distance_sq = distance_sq;
    }
  }

  if (nearest_distance_sq < (circle_shape.radius * circle_shape.radius)) {
    return {polygon_points[nearest_vertex], circle.position, true};
  }

  // Check edges
  int nearest_edge = -1;
  float closest_projection = std::numeric_limits<float>().lowest();

  for (int i = 0; i < polygon_points.size(); ++i) {
    const int next = (i + 1) % polygon_points.size();
    const math::Vec2 edge = polygon_points[next] - polygon_points[i];

    // Check if it's within the line segment
    if (math::dot(circle.position - polygon_points[i], edge) < 0) {
      continue;
    }

    if (math::dot(circle.position - polygon_points[next], -edge) < 0) {
      continue;
    }

    const math::Vec2 edge_normal = (polygon_points[next] - polygon_points[i]).normal().normalized();
    const float projection = math::dot(circle.position - polygon_points[i], edge_normal);

    const bool better = (closest_projection < 0 && projection >= 0)
                        || (closest_projection >= 0 && projection >= 0 && projection < closest_projection)
                        || (closest_projection < 0 && projection < 0 && projection > closest_projection);

    if (better) {
      nearest_edge = i;
      closest_projection = projection;
    }
  }

  if (nearest_edge == -1 || closest_projection > circle_shape.radius || (!is_inside && closest_projection < 0)) {
    return {{}, {}, false};
  }

  const int next = (nearest_edge + 1) % polygon_points.size();

  return {polygon_points[nearest_edge], polygon_points[next], true};
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

    if (state.primary == input::ButtonState::Down) {
      bodies[1].position = pointer;
    }

    if (state.secondary == input::ButtonState::Down) {
      bodies[2].position = pointer;
    }

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

    const auto circle1 = bodies[1];
    const auto circle2 = bodies[2];
    const auto polygon = bodies[0];

    auto [start2, end2, has_collision2] = test_circle_polygon_vertex_and_edge(circle2, polygon);

    if (has_collision2) {
      drawer.line(fb, start2, end2, render::color::red);
      drawer.line(fb, start2, circle2.position, render::color::purple);
    }

    auto [start1, end1, has_collision1] = test_circle_polygon_vertex_and_edge(circle1, polygon);

    if (has_collision1) {
      drawer.line(fb, start1, end1, render::color::red);
      drawer.line(fb, start1, circle1.position, render::color::purple);
    }
  };

  return engine::run(app_config, read_input, update, render);
}

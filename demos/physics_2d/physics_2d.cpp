#include "drawer.h"
#include "engine/run.h"
#include "engine/sdl.h"
#include "input/input.h"
#include "math/rect.h"
#include "physics/body.h"
#include "physics/force.h"
#include "render/color.h"
#include "render/framebuffer.h"

#include <set>

int main(int argc, char* argv[]) {
  constexpr engine::SdlSettings settings{
      .width = 960,
      .height = 720,
      .scale = 1,
      .enable_v_sync = true,
  };

  constexpr engine::AppConfig app_config = {
      .sdl_settings = settings,
      .background = render::color::near_black,
  };

  constexpr float pixels_per_meter = 10.0f;

  Drawer drawer(pixels_per_meter);

  constexpr float world_width = settings.width / pixels_per_meter;
  constexpr float world_height = settings.height / pixels_per_meter;

  bool is_holding = false;
  int held_particle = 0;
  math::Vec2 pointer;

  std::vector<physics::Body> bodies{};
  std::set<std::pair<int, int>> connections{};

  constexpr int width = 8;
  constexpr int height = 8;
  constexpr float x_pitch = 5.0f;
  constexpr float y_pitch = 5.0f;
  constexpr float y0 = 25.0f;
  constexpr float x0 = 25.0f;

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      const math::Vec2 pos{.x = x0 + x * x_pitch, .y = y0 + y * y_pitch};

      // const float mass = (y + 1) == height && (x == 0 || (x + 1) == width) ? 0 : 0.1f;
      const float mass = (y + 1) == height ? 0 : 0.5f;
      // const float mass = x == 0 ? 0 : 0.05f;
      // constexpr float mass = 0.1f;

      bodies.emplace_back(physics::Body(mass, physics::shape::Circle{}, pos));
    }
  }

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      const int current = y * height + x;
      const int next_row = (y + 1) * height + x;

      const bool top = y + 1 == width;
      const bool right = x + 1 == height;
      const bool left = x == 0;

      if (!right)
        connections.emplace(current, current + 1);
      if (!top)
        connections.emplace(current, next_row);
      if (!top && !right)
        connections.emplace(current, next_row + 1);
      if (!top && !left)
        connections.emplace(current, next_row - 1);
    }
  }

  auto update = [&bodies, &connections](const float dt, const input::InputState& input) {
    constexpr physics::force::SpringParams spring_params{.rest_length = 6, .k = 1, .c = 2};

    for (auto& [a_index, b_index] : connections) {
      auto& a = bodies[a_index];
      auto& b = bodies[b_index];

      const math::Vec2 force = physics::force::spring(a, b, spring_params);

      a.add_force(force);
      b.add_force(-force);
    }

    for (auto& body : bodies) {
      body.add_force(physics::force::gravity(body.mass));
    }

    for (auto& body : bodies) {
      body.integrate(dt);
    }

    for (auto& body : bodies) {
      // Note: not real physics for now
      constexpr float restitution = 0.9f;

      if (std::holds_alternative<physics::shape::Circle>(body.shape)) {
        auto circle = std::get<physics::shape::Circle>(body.shape);

        if (body.position.x - circle.radius <= 0) {
          body.position.x = circle.radius;
          body.velocity.x *= -restitution;
        } else if (body.position.x + circle.radius >= world_width) {
          body.position.x = world_width - circle.radius;
          body.velocity.x *= -restitution;
        }

        if (body.position.y - circle.radius <= 0) {
          body.position.y = circle.radius;
          body.velocity.y *= -restitution;
        } else if (body.position.y + circle.radius >= world_height) {
          body.position.y = world_height - circle.radius;
          body.velocity.y *= -restitution;
        }
      }
    }
  };

  auto read_input = [&bodies, &pointer, &is_holding, &held_particle](const input::InputState& state,
                                                                     const input::InputEvents& events) {
    pointer = {.x = state.cursor_position.x / pixels_per_meter,
               .y = (settings.height - state.cursor_position.y) / pixels_per_meter};

    if (events.primary == input::Event::Pressed) {
      for (int i = 0; i < bodies.size(); ++i) {
        const auto p = bodies[i];

        if ((pointer - p.position).length() <= 1) {
          is_holding = true;
          held_particle = i;
          break;
        }
      }
    }

    if (events.primary == input::Event::Released) {
      if (is_holding) {
        bodies[held_particle].velocity = pointer - bodies[held_particle].position;
      }

      is_holding = false;
    }
  };

  auto render = [&bodies, &connections, &is_holding, &held_particle, &pointer, &drawer](render::Framebuffer& fb) {
    if (is_holding) {
      drawer.line(fb, bodies[held_particle].position, pointer, render::color::red);
    }

    drawer.filled_circle(fb, pointer, 1, render::color::red);

    for (auto& [a, b] : connections) {
      drawer.line(fb, bodies[a].position, bodies[b].position, render::color::white);
    }

    for (auto& p : bodies) {
      drawer.filled_circle(fb, p.position, 1, render::color::white);
    }
  };

  return engine::run(app_config, read_input, update, render);
}

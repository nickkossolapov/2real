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

namespace {

math::Vec2 to_world(const math::Vec2 center, const math::Vec2 local, const float a) {
  // left-handed rotation matrix
  const float x = std::cos(a) * local.x + std::sin(a) * local.y;
  const float y = -std::sin(a) * local.x + std::cos(a) * local.y;

  return center + math::Vec2{x, y};
}

void render_body(const Drawer& drawer, render::Framebuffer& fb, const physics::Body& body) {
  auto visitor = Overloaded{
      [&](const physics::shape::Circle& c) {
        drawer.debug_circle(fb, body.position, c.radius, body.rotation, render::color::white);
      },
      [&](const physics::shape::Box& b) {
        for (int i = 0; i < 4; ++i) {
          const int next = (i + 1) % 4;

          drawer.line(fb,
                      to_world(body.position, b.points[i], body.rotation),
                      to_world(body.position, b.points[next], body.rotation),
                      render::color::white);
        }
      },
  };

  return std::visit(visitor, body.shape);
}

} // namespace

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

  constexpr float pixels_per_meter = 20.0f;

  Drawer drawer(pixels_per_meter);

  constexpr float world_width = settings.width / pixels_per_meter;
  constexpr float world_height = settings.height / pixels_per_meter;

  bool is_holding = false;
  int held_particle = 0;
  math::Vec2 pointer;

  std::vector<physics::Body> bodies{};

  bodies.emplace_back(1.0f, physics::shape::Circle(2.0f));
  physics::Body& circle = bodies[0];
  circle.position = {20, 20};
  circle.add_torque(1.0f);

  bodies.emplace_back(1.0f, physics::shape::Box(3.0f, 2.0f));
  physics::Body& box = bodies[1];
  box.position = {10, 20};
  box.add_torque(1.0f);

  auto update = [&bodies](const float dt, const input::InputState& input) {
    for (auto& body : bodies) {
      // body.add_force(physics::force::gravity(body.mass));
    }

    for (auto& body : bodies) {
      body.integrate(dt);
    }

    for (auto& body : bodies) {
      // Note: not real physics for now
      constexpr float restitution = 0.9f;

      if (std::holds_alternative<physics::shape::Circle>(body.shape)) {
        auto [radius] = std::get<physics::shape::Circle>(body.shape);

        if (body.position.x - radius <= 0) {
          body.position.x = radius;
          body.velocity.x *= -restitution;
        } else if (body.position.x + radius >= world_width) {
          body.position.x = world_width - radius;
          body.velocity.x *= -restitution;
        }

        if (body.position.y - radius <= 0) {
          body.position.y = radius;
          body.velocity.y *= -restitution;
        } else if (body.position.y + radius >= world_height) {
          body.position.y = world_height - radius;
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

  auto render = [&bodies, &is_holding, &held_particle, &pointer, &drawer](render::Framebuffer& fb) {
    if (is_holding) {
      drawer.line(fb, bodies[held_particle].position, pointer, render::color::red);
    }

    for (auto& body : bodies) {
      render_body(drawer, fb, body);
    }
  };

  return engine::run(app_config, read_input, update, render);
}

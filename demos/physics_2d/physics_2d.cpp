#include "drawer.h"
#include "engine/run.h"
#include "engine/sdl.h"
#include "input/input.h"
#include "math/rect.h"
#include "physics/body.h"
#include "physics/collision.h"
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

void render_body(const Drawer& drawer, render::Framebuffer& fb, const physics::Body& body, uint32_t color) {
  auto visitor = Overloaded{
      [&](const physics::shape::Circle& c) { drawer.debug_circle(fb, body.position, c.radius, body.rotation, color); },
      [&](const physics::shape::Box& b) {
        for (int i = 0; i < 4; ++i) {
          const int next = (i + 1) % 4;

          drawer.line(fb,
                      to_world(body.position, b.points[i], body.rotation),
                      to_world(body.position, b.points[next], body.rotation),
                      color);
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

  bodies.emplace_back(2.0f, physics::shape::Circle(2.0f));
  physics::Body& circle1 = bodies[0];
  circle1.position = {20, 20};

  bodies.emplace_back(3.0f, physics::shape::Circle(3.0f));
  physics::Body& circle2 = bodies[1];
  circle2.position = {10, 20};

  std::optional<physics::Contact> contact;

  auto update = [&bodies, &contact](const float dt, const input::InputState& input) {
    // constexpr math::Vec2 wind = {10, 0};
    //
    // for (auto& body : bodies) {
    //   body.add_force(physics::force::gravity(body.mass));
    //   body.add_force(physics::force::drag(body.velocity - wind, 0.1));
    // }

    for (auto& body : bodies) {
      body.integrate(dt);
    }

    for (int i = 0; i < bodies.size(); ++i) {
      for (int j = i + 1; j < bodies.size(); ++j) {
        auto a = bodies[i];
        auto b = bodies[j];

        contact.reset();

        if (auto c = physics::collision::test(a, b))
          contact.emplace(*c);
      }
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

    bodies[0].position = pointer;

    if (events.primary == input::Event::Released) {
      if (is_holding) {
        bodies[held_particle].velocity = pointer - bodies[held_particle].position;
      }

      is_holding = false;
    }
  };

  auto render = [&bodies, &is_holding, &held_particle, &pointer, &drawer, &contact](render::Framebuffer& fb) {
    if (is_holding) {
      drawer.line(fb, bodies[held_particle].position, pointer, render::color::red);
    }

    const uint32_t color = contact.has_value() ? render::color::red : render::color::white;

    if (contact.has_value()) {
      drawer.filled_circle(fb, contact->start, 0.2, render::color::red);
      drawer.filled_circle(fb, contact->end, 0.2, render::color::red);
      drawer.line(fb, contact->start, contact->start + contact->normal, render::color::white);
    }

    for (auto& body : bodies) {
      render_body(drawer, fb, body, color);
    }
  };

  return engine::run(app_config, read_input, update, render);
}

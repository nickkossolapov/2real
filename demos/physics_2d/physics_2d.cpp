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

void render_body(const Drawer& drawer, render::Framebuffer& fb, const physics::Body& body, uint32_t color) {
  auto visitor = Overloaded{
      [&](const physics::shape::Circle& c) { drawer.filled_circle(fb, body.position, c.radius, color); },
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

  bodies.emplace_back(1.0f, physics::shape::box(4.0f, 4.0f), math::Vec2{30.0f, 18.0f});
  bodies.emplace_back(1.0f, physics::shape::box(4.0f, 4.0f), math::Vec2{10.0f, 18.0f});

  bodies[0].angular_velocity = 0.1f;
  bodies[1].angular_velocity = 0.4f;

  std::optional<physics::Contact> contact;

  auto update = [&bodies, &contact](const float dt, const input::InputState& input) {
    // constexpr math::Vec2 wind = {10, 0};
    //
    // for (auto& body : bodies) {
    //   body.add_force(physics::force::gravity(body.mass));
    //   body.add_force(physics::force::friction(body.velocity, 0.003));
    // }

    for (auto& body : bodies) {
      body.integrate(dt);
    }

    for (int i = 0; i < bodies.size(); ++i) {
      for (int j = i + 1; j < bodies.size(); ++j) {
        auto& a = bodies[i];
        auto& b = bodies[j];

        contact.reset();

        if (auto c = physics::collision::test(a, b)) {
          contact.emplace(*c);

          // physics::resolution::resolve(a, b, *c);
        }
      }
    }
  };

  auto read_input = [&bodies, &pointer, &is_holding, &held_particle](const input::InputState& state,
                                                                     const input::InputEvents& events) {
    pointer = {.x = state.cursor_position.x / pixels_per_meter,
               .y = (settings.height - state.cursor_position.y) / pixels_per_meter};

    // if (events.primary == input::Event::Pressed) {
    //   for (int i = 0; i < bodies.size(); ++i) {
    //     const auto p = bodies[i];
    //
    //     auto [radius] = std::get<physics::shape::Circle>(p.shape);
    //
    //     if ((pointer - p.position).length() <= radius) {
    //       is_holding = true;
    //       held_particle = i;
    //       break;
    //     }
    //   }
    // }

    if (state.primary == input::ButtonState::Down) {
      bodies[0].position = pointer;
    }

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

    // if (contact.has_value()) {
    //   drawer.filled_circle(fb, contact->start, 0.2, render::color::red);
    //   drawer.filled_circle(fb, contact->end, 0.2, render::color::red);
    //   drawer.line(fb, contact->start, contact->start + contact->normal, render::color::white);
    // }

    for (auto& body : bodies) {
      render_body(drawer, fb, body, color);
    }
  };

  return engine::run(app_config, read_input, update, render);
}

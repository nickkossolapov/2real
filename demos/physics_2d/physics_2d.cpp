#include "engine/run.h"
#include "engine/sdl.h"
#include "input/input.h"
#include "math/rect.h"
#include "physics/force.h"
#include "physics/particle.h"
#include "render/color.h"
#include "render/draw.h"
#include "render/framebuffer.h"

int main(int argc, char* argv[]) {
  constexpr engine::SdlSettings settings{
      .width = 1280,
      .height = 960,
      .scale = 1,
      .enable_v_sync = true,
  };

  constexpr engine::AppConfig app_config = {
      .sdl_settings = settings,
      .background = render::color::near_black,
  };

  constexpr float pixels_per_meter = 10.0f;

  constexpr float world_width = settings.width / pixels_per_meter;
  constexpr float world_height = settings.height / pixels_per_meter;

  std::vector<physics::Particle> particles;

  particles.emplace_back(physics::Particle(1, 1, {1, 2}));
  // particles.emplace_back(physics::Particle(3, 3, {2, 2}));

  constexpr math::Vec2 wind = {.x = 100};
  constexpr math::Vec2 gravity = {.y = 9.8};

  constexpr math::Rect liquid = {
      0,
      world_width,
      world_height / 2,
      world_height,
  };

  auto update = [&particles, &gravity, &wind, &liquid](const float dt, const input::InputState& input) {
    for (auto& p : particles) {
      p.add_force(gravity * p.mass);

      if (liquid.contains(p.position)) {
        const math::Vec2 drag = physics::generate_drag_force(p.velocity, 0.1f);

        p.add_force(drag);
      } else {
        const math::Vec2 wind_drag = physics::generate_drag_force(p.velocity - wind, 0.0005f);

        p.add_force(wind_drag);
      }
    }

    for (auto& p : particles) {
      p.integrate(dt);
    }

    for (auto& p : particles) {
      // Note: not real physics for now
      constexpr float restitution = 0.9f;

      if (p.position.x - p.radius <= 0) {
        p.position.x = p.radius;
        p.velocity.x *= -restitution;
      } else if (p.position.x + p.radius >= world_width) {
        p.position.x = world_width - p.radius;
        p.velocity.x *= -restitution;
      }

      if (p.position.y - p.radius <= 0) {
        p.position.y = p.radius;
        p.velocity.y *= -restitution;
      } else if (p.position.y + p.radius >= world_height) {
        p.position.y = world_height - p.radius;
        p.velocity.y *= -restitution;
      }
    }
  };

  auto read_input = [&particles](const input::InputState& state, const input::InputEvents& events) {
    if (events.primary == input::Event::Pressed) {
      const math::Vec2 pos = state.cursor_position / pixels_per_meter;

      particles.emplace_back(physics::Particle(1, 1, pos));
    }
  };

  auto render = [&particles](render::Framebuffer& fb) {
    render::draw::rect(fb,
                       {liquid.x_min * pixels_per_meter, liquid.y_min * pixels_per_meter},
                       (liquid.x_max - liquid.x_min) * pixels_per_meter,
                       (liquid.y_max - liquid.y_min) * pixels_per_meter,
                       render::color::sky_blue);

    for (auto& p : particles) {
      render::draw::filled_circle(fb, p.position * pixels_per_meter, p.radius * pixels_per_meter, render::color::white);
    }
  };

  return engine::run(app_config, read_input, update, render);
}

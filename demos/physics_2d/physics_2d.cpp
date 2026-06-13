#include "engine/run.h"
#include "engine/sdl.h"
#include "input/input.h"
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

  std::vector<physics::Particle> particles;

  particles.emplace_back(physics::Particle(1, 1, {1, 2}));
  particles.emplace_back(physics::Particle(3, 3, {2, 2}));

  constexpr math::Vec2 wind = {.x = 100};
  constexpr math::Vec2 gravity = {.y = 9.8};

  auto update = [&particles, &gravity, &wind](const float dt, const input::State& input) {
    for (auto& p : particles) {
      const math::Vec2 drag = (p.velocity - wind) * -0.1;

      p.add_force(drag);
    }

    for (auto& p : particles) {
      p.add_force(gravity * p.mass);
    }

    for (auto& p : particles) {
      p.integrate(dt);
    }
  };

  auto render = [&particles](render::Framebuffer& fb) {
    for (auto& p : particles) {
      render::draw::filled_circle(fb, p.position * pixels_per_meter, p.radius * pixels_per_meter, render::color::white);
    }
  };

  return engine::run(app_config, update, render);
}

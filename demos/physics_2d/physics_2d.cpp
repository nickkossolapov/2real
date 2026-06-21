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

  const physics::Particle anchor(0, 1, {50, 20});
  physics::Particle particle(1, 1, {50, 50});

  auto update = [&particle, &anchor](const float dt, const input::InputState& input) {
    // particle.add_force(physics::force::spring(particle.position, anchor.position, 20, 2));
    particle.add_force(physics::force::damped_spring(particle.position,
                                                     anchor.position,
                                                     particle.velocity - anchor.velocity,
                                                     20,
                                                     0.02,
                                                     5));

    particle.integrate(dt);

    // for (auto& p : particles) {
    //   p.integrate(dt);
    // }
    //
    // for (auto& p : particles) {
    //   // Note: not real physics for now
    //   constexpr float restitution = 0.9f;
    //
    //   if (p.position.x - p.radius <= 0) {
    //     p.position.x = p.radius;
    //     p.velocity.x *= -restitution;
    //   } else if (p.position.x + p.radius >= world_width) {
    //     p.position.x = world_width - p.radius;
    //     p.velocity.x *= -restitution;
    //   }
    //
    //   if (p.position.y - p.radius <= 0) {
    //     p.position.y = p.radius;
    //     p.velocity.y *= -restitution;
    //   } else if (p.position.y + p.radius >= world_height) {
    //     p.position.y = world_height - p.radius;
    //     p.velocity.y *= -restitution;
    //   }
    // }
  };

  auto read_input = [](const input::InputState& state, const input::InputEvents& events) {};

  auto render = [&particle, &anchor](render::Framebuffer& fb) {
    render::draw::line(fb,
                       particle.position * pixels_per_meter,
                       anchor.position * pixels_per_meter,
                       render::color::white);

    render::draw::filled_circle(fb,
                                particle.position * pixels_per_meter,
                                particle.radius * pixels_per_meter,
                                render::color::white);

    render::draw::filled_circle(fb,
                                anchor.position * pixels_per_meter,
                                anchor.radius * pixels_per_meter,
                                render::color::sky_blue);
  };

  return engine::run(app_config, read_input, update, render);
}

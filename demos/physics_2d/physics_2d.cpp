#include "drawer.h"
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

  Drawer drawer(pixels_per_meter);

  constexpr float world_width = settings.width / pixels_per_meter;
  constexpr float world_height = settings.height / pixels_per_meter;

  bool is_holding = false;
  math::Vec2 pointer;

  const physics::Particle anchor(0, 1, {50, 50});
  physics::Particle particle(1, 1, {50, 30});

  auto update = [&particle, &anchor](const float dt, const input::InputState& input) {
    particle.add_force(physics::force::spring(particle, anchor, {.rest_length = 20, .k = 5, .c = 0.1}));

    particle.add_force(physics::force::gravity(particle.mass));

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

  auto read_input = [&particle, &pointer, &is_holding](const input::InputState& state,
                                                       const input::InputEvents& events) {
    pointer = {.x = state.cursor_position.x / pixels_per_meter,
               .y = (settings.height - state.cursor_position.y) / pixels_per_meter};

    if (events.primary == input::Event::Pressed) {
      if ((pointer - particle.position).length() <= particle.radius) {
        is_holding = true;
      }
    }

    if (events.primary == input::Event::Released) {
      if (is_holding) {
        particle.velocity = particle.position - pointer;
      }

      is_holding = false;
    }
  };

  auto render = [&particle, &anchor, &is_holding, &pointer, &drawer](render::Framebuffer& fb) {
    if (is_holding) {
      drawer.line(fb, particle.position, pointer, render::color::red);
    }

    drawer.filled_circle(fb, pointer, 1, render::color::red);

    drawer.line(fb, particle.position, anchor.position, render::color::white);

    drawer.filled_circle(fb, particle.position, particle.radius, render::color::white);

    drawer.filled_circle(fb, anchor.position, anchor.radius, render::color::sky_blue);
  };

  return engine::run(app_config, read_input, update, render);
}

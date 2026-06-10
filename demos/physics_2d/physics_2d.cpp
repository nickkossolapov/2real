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

  physics::Particle particle;

  particle.position.x = 50;
  particle.position.y = 50;

  auto update = [&particle](const float dt, const input::State& input) { particle.position += {2 * dt, 2 * dt}; };

  auto render = [&particle](render::Framebuffer& fb) {
    render::draw::filled_circle(fb, particle.position, 50, render::color::white);
  };

  return engine::run(app_config, update, render);
}

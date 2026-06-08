#include "engine/run.h"
#include "engine/sdl.h"
#include "input/input.h"
#include "render/color.h"
#include "render/framebuffer.h"

int main(int argc, char* argv[]) {
  constexpr engine::SdlSettings settings{
      .width = 320,
      .height = 240,
      .scale = 4,
      .enable_v_sync = true,
  };

  constexpr engine::AppConfig app_config = {.sdl_settings = settings};

  auto update = [](float, const input::State&) {};
  auto render = [](render::Framebuffer&) {};

  engine::run(app_config, update, render);
}

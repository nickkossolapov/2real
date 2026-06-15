#pragma once

#include "input/input.h"
#include "render/color.h"
#include "render/framebuffer.h"
#include "sdl.h"

#include <functional>

namespace engine {

struct AppConfig {
  SdlSettings sdl_settings;
  float target_fps = 60.0f;
  uint32_t background = render::color::near_black;
  float fixed_timestep = 0.005f; // TODO add accumulator
};

int run(const AppConfig& cfg,
        const std::function<void(float dt, const input::Snapshot&)>& update,
        const std::function<void(render::Framebuffer&)>& render);

} // namespace engine

#include "run.h"

#include "fps_logger.h"
#include "timing.h"

namespace engine {

namespace {

constexpr float max_accumulator_time = 0.25f;

}

int run(const AppConfig& cfg,
        const std::function<void(const input::InputState&, const input::InputEvents&)>& read_input,
        const std::function<void(float dt, const input::InputState&)>& update,
        const std::function<void(render::Framebuffer&)>& render) {
  SdlContext sdl;

  if (auto err = sdl.init(cfg.sdl_settings); err != InitError::None) {
    return static_cast<int>(err);
  }

  bool quit = false;

  auto framebuffer = render::Framebuffer(cfg.sdl_settings.width, cfg.sdl_settings.height, cfg.background);
  auto frame_limiter = FrameLimiter(cfg.target_fps, cfg.sdl_settings.enable_v_sync);

  input::InputState input_state;
  input::InputEvents input_events;
  input::KeyboardState kb_state;
  FpsLogger fps_logger;

  float accumulator = 0.0f;

  while (!quit) {
    const float frame_time = frame_limiter.tick() / 1000.0f; // Convert to seconds

    quit = input::process_input(sdl.gamepad(), input_state, input_events, kb_state);
    read_input(input_state, input_events);

    accumulator += std::min(frame_time, max_accumulator_time);

    while (accumulator > 0.0f) {
      update(cfg.fixed_timestep, input_state);
      accumulator -= cfg.fixed_timestep;
    }

    render(framebuffer);

    framebuffer.present(sdl.renderer(), sdl.display_texture());
    fps_logger.tick();
  }

  return 0;
}

} // namespace engine
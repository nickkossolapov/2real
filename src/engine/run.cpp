#include "run.h"

#include "fps_logger.h"
#include "timing.h"

namespace engine {

int run(const AppConfig& cfg,
        const std::function<void(float dt, const input::State&)>& update,
        const std::function<void(render::Framebuffer&)>& render) {
  SdlContext sdl;

  if (auto err = sdl.init(cfg.sdl_settings); err != InitError::None) {
    return static_cast<int>(err);
  }

  bool quit = false;

  auto framebuffer = render::Framebuffer(cfg.sdl_settings.width, cfg.sdl_settings.height, cfg.background);
  auto frame_limiter = FrameLimiter(cfg.target_fps, cfg.sdl_settings.enable_v_sync);

  input::State input_state;
  FpsLogger fps_logger;

  while (!quit) {
    const float dt = frame_limiter.tick() / 1000.0f; // Convert to seconds

    quit = input::process_input(dt, sdl.gamepad(), input_state);
    update(dt, input_state);

    render(framebuffer);

    framebuffer.present(sdl.renderer(), sdl.display_texture());
    fps_logger.tick();
  }

  return 0;
}

} // namespace engine
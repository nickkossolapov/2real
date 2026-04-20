#include "sdl.h"

namespace engine {

InitError SdlContext::init(const bool enable_v_sync) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
    return InitError::SDLInit;
  }

  SDL_Window* raw_window = nullptr;
  SDL_Renderer* raw_renderer = nullptr;

  if (!SDL_CreateWindowAndRenderer("2real", window::width, window::height, 0, &raw_window, &raw_renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return InitError::WindowCreate;
  }

  window_.reset(raw_window);
  renderer_.reset(raw_renderer);

  if (enable_v_sync) {
    if (!SDL_SetRenderVSync(renderer_.get(), 1)) {
      SDL_Log("Could not enable VSync! SDL error: %s\n", SDL_GetError());
      return InitError::VSyncEnable;
    }
  }

  auto* texture = SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING, window::width, window::height);

  if (texture == nullptr) {
    SDL_Log("Couldn't create texture: %s", SDL_GetError());
    return InitError::DisplayTextureCreate;
  }

  display_texture_.reset(texture);

  if (int count = 0; SDL_JoystickID* ids = SDL_GetGamepads(&count)) {
    if (count > 0) {
      auto* gamepad = SDL_OpenGamepad(ids[0]);

      if (gamepad == nullptr) {
        SDL_Log("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
      }

      controller_.reset(gamepad);
    } else {
      SDL_Log("Warning: No gamepads connected!\n");
    }
    SDL_free(ids);
  }

  return InitError::None;
}

SdlContext::~SdlContext() {
  controller_.reset();
  display_texture_.reset();
  renderer_.reset();
  window_.reset();
  SDL_Quit();
}

} // namespace engine

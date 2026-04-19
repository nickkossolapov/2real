#include "init.h"

namespace engine {

InitError init_sdl(SDL_Window*& window,
                   SDL_Renderer*& renderer,
                   SDL_Texture*& display_texture,
                   SDL_Gamepad*& controller,
                   const bool enable_v_sync) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    SDL_Log("SDL_Init Error: %s\n", SDL_GetError());

    return InitError::SDLInit;
  }

  if (!SDL_CreateWindowAndRenderer("2real", window::width, window::height, 0, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());

    return InitError::WindowCreate;
  }

  if (enable_v_sync) {
    if (SDL_SetRenderVSync(renderer, 1) == false) {
      SDL_Log("Could not enable VSync! SDL error: %s\n", SDL_GetError());

      return InitError::VSyncEnable;
    }
  }

  display_texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window::width, window::height);

  if (display_texture == nullptr) {
    SDL_Log("Couldn't create texture: %s", SDL_GetError());

    return InitError::DisplayTextureCreate;
  }

  if (int count = 0; SDL_JoystickID* ids = SDL_GetGamepads(&count)) {
    if (count > 0) {
      controller = SDL_OpenGamepad(ids[0]);

      if (controller == nullptr) {
        SDL_Log("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
      }
    } else {
      SDL_Log("Warning: No gamepads connected!\n");
    }
    SDL_free(ids);
  }

  return InitError::None;
}

} // namespace engine
